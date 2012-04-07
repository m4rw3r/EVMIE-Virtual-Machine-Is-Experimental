
#include "Token.h"

#define CHAR_BUF_INITIAL_SIZE 8

#define CDECL_CHAR_BUF(buf, initialsize)            \
	char *buf = malloc(sizeof(char) * initialsize); \
	int buf ## _size = initialsize;                 \
	int buf ## _idx = 0;

#define RESIZE_CHAR_BUF(buf, size)                              \
	if(size >= buf ## _size) {                                  \
		buf ## _size *= 2;                                      \
		buf = realloc(buf, sizeof(char) * buf ## _size);        \
		if( ! buf) {                                            \
			SrcFile_error(f, "Failed to alloc token buffer with size %d", buf ## _size); \
			return NULL; }}

#define BUF_PUT(buf, chr)              \
	RESIZE_CHAR_BUF(buf, buf ## _idx); \
	buf[buf ## _idx] = chr;            \
	buf ## _idx++;

#define LITERAL1(chr, token) \
	if(c == chr) { SrcFile_count(f, c); free(buf); return Token_fromType(token); }

#define LITERAL2(chr, str, token)  \
	if(c == chr && SrcFile_testStr(f, str)) { \
		SrcFile_count(f, c); free(buf); return Token_fromType(token); \
	}

/**
 * Returns C tokens one at a time, only supports a subset of C.
 * 
 * Implemented (hopefully mostly correctly) as specified in
 * http://www.lysator.liu.se/c/ANSI-C-grammar-l.html
 */
Token *Token_nextToken(SrcFile *f, int return_whitespace)
{
	char c;
	CDECL_CHAR_BUF(buf, CHAR_BUF_INITIAL_SIZE);
	
	find_token:
	/* Eat all whitespace */
	if( ! return_whitespace) {
		/* TODO: Is it ok to eat \0 ? */
		while((c = SrcFile_getc(f)) != EOF && IS_W(c)) {
			SrcFile_count(f, c);
		}
	} else if((c = SrcFile_getc(f)) != EOF && IS_W(c)) {
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		while((c = SrcFile_getc(f)) != EOF && IS_W(c)) {
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_Whitespace(buf);
	}
	
	if(c == EOF){
		free(buf);
		
		return Token_fromType(T_EOF);
	}
	
	/* Comment: "/ *" */
	if(c == '/' && SrcFile_testc(f, '*'))
	{
		SrcFile_count(f, c);
		SrcFile_count(f, '*');
		char c1;
		
		eat_comment:
		while((c = SrcFile_getc(f)) != EOF && c != '*') {
			SrcFile_count(f, c);
		}
		
		SrcFile_count(f, c);
		
		if((c1 = SrcFile_getc(f)) != EOF && c1 != '/') {
			SrcFile_ungetc(f, c1);
			goto eat_comment;
		}
		
		SrcFile_count(f, c1);
		
		/* Skip comments */
		goto find_token;
	}
	
	/* TODO: Parse some macros? */
	if(c == '#')
	{
		char c1;
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		eat_macro:
		while((c = SrcFile_getc(f)) != EOF && c != '\n' && c != '\\') {
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		if(c == '\\') {
			if((c1 = SrcFile_getc(f)) != EOF && c1 == '\n') {
				SrcFile_count(f, c);
				SrcFile_count(f, c1);
				BUF_PUT(buf, c1);
				goto eat_macro;
			} else {
				SrcFile_ungetc(f, c1);
			}
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_Macro(buf);
	}
	
	/* Identifier: {L}({L}|{D})* */
	if(IS_L(c))
	{
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		while((c = SrcFile_getc(f)) != EOF && (IS_L(c) || IS_D(c)))
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_Identifier(buf);
	}
	
	/* TODO: Fix {IS} support? */
	/* Ignoring {IS} on lines below */
	/* Constant Hexadecimal: 0[Xx]{H}+{IS} */
	if(c == '0' && (SrcFile_testc(f, 'x') || SrcFile_testc(f, 'X')))
	{
		SrcFile_count(f, c);
		SrcFile_count(f, 'x');
		
		c = SrcFile_getc(f);
		
		if( ! IS_H(c)) {
			SrcFile_error(f, "Hexadecimal number expected after \"0x\"");
			
			free(buf);
			return NULL;
		}
		
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		while((c = SrcFile_getc(f)) != EOF && IS_H(c))
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_HexNum(buf);
	}

	/* Octal number  0{D}+ */
	if(c == '0')
	{
		SrcFile_count(f, c);
		c = SrcFile_getc(f);
		
		if( ! IS_D(c)) {
			/* Just a plain zero */
			BUF_PUT(buf, '0');
			BUF_PUT(buf, '\0');
			SrcFile_ungetc(f, c);
			
			return Token_DecNum(buf);
		}
		
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		while((c = SrcFile_getc(f)) != EOF && IS_D(c))
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_OctNum(buf);
	}
	
	/* Decimal number {D}+ */
	if(IS_D(c))
	{
		SrcFile_count(f, c);
		BUF_PUT(buf, c);
		
		while((c = SrcFile_getc(f)) != EOF && IS_D(c))
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_DecNum(buf);
	}
	
	/* Char constant L?'(\\.|[^\\'])+' DOES NOT SUPPORT THE L? PART */
	if(c == '\'')
	{
		SrcFile_count(f, c);
		if((c = SrcFile_getc(f)) == '\\') {
			char c1;
			SrcFile_count(f, c);
			if((c1 = SrcFile_getc(f)) == EOF) {
				goto error_constchr_eof;
			}
			BUF_PUT(buf, c);
			SrcFile_count(f, c1);
			BUF_PUT(buf, c1);
		} else {
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		if((c = SrcFile_getc(f)) != '\'') {
			goto error_constchr_expectend;
		}
		
		BUF_PUT(buf, '\0');
		
		return Token_ConstChr(buf);
	}
	
	/* TODO: Fix the comment below? */
	/* Ignoring the
	   {D}+{E}{FS}?
	   {D}*"."{D}+({E})?{FS}?
	   {D}+"."{D}*({E})?{FS}?
	   T_CONSTANT rules for now */
	
	/* Constant String "(\.|[^\"])*" */
	if(c == '"')
	{
		SrcFile_count(f, c);
		parse_conststr:
		/* \. */
		if((c = SrcFile_getc(f)) == '\\') {
			char c1;
			
			SrcFile_count(f, c);
			if((c1 = SrcFile_getc(f)) == EOF) {
				goto error_conststr_eof;
			}
			
			BUF_PUT(buf, c);
			SrcFile_count(f, c1);
			BUF_PUT(buf, c1);
			
			goto parse_conststr;
		}
		
		if(c == EOF) {
			goto error_conststr_eof;
		}
		
		if(c != '"')
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
			
			goto parse_conststr;
		}
		else
		{
			/* c == '"' */
			BUF_PUT(buf, '\0');
			return Token_ConstStr(buf);
		}
	}
	
	LITERAL2('.', "..", T_ELLIPSIS);
	LITERAL2('>', ">=", T_RIGHT_ASSIGN);
	LITERAL2('<', "<=", T_LEFT_ASSIGN);
	LITERAL2('+', "=",  T_ADD_ASSIGN);
	LITERAL2('-', "=",  T_SUB_ASSIGN);
	LITERAL2('*', "=",  T_MUL_ASSIGN);
	LITERAL2('/', "=",  T_DIV_ASSIGN);
	LITERAL2('%', "=",  T_MOD_ASSIGN);
	LITERAL2('&', "=",  T_AND_ASSIGN);
	LITERAL2('^', "=",  T_XOR_ASSIGN);
	LITERAL2('|', "=",  T_OR_ASSIGN);
	LITERAL2('>', ">",  T_RIGHT_OP);
	LITERAL2('<', "<",  T_LEFT_OP);
	LITERAL2('+', "+",  T_INC_OP);
	LITERAL2('-', "-",  T_DEC_OP);
	LITERAL2('-', ">",  T_PTR_OP);
	LITERAL2('&', "&",  T_AND_OP);
	LITERAL2('|', "|",  T_OR_OP);
	LITERAL2('<', "=",  T_LE_OP);
	LITERAL2('>', "=",  T_GE_OP);
	LITERAL2('=', "=",  T_EQ_OP);
	LITERAL2('!', "=",  T_NE_OP);
	LITERAL1(';',       T_SEMICOLON);
	LITERAL1('{', T_BLOCKSTART);
	LITERAL1('}', T_BLOCKEND);
	LITERAL1(',', T_COMMA);
	LITERAL1(':', T_COLON);
	LITERAL1('=', T_EQUALS);
	LITERAL1('(', T_LPAREN);
	LITERAL1(')', T_RPAREN);
	LITERAL1('[', T_LBRACKET);
	LITERAL1(']', T_RBRACKET);
	LITERAL1('.', T_DOT);
	LITERAL1('&', T_BW_AND);
	LITERAL1('!', T_NOT);
	LITERAL1('~', T_BW_NOT);
	LITERAL1('-', T_MINUS);
	LITERAL1('+', T_PLUS);
	LITERAL1('*', T_MUL);
	LITERAL1('/', T_DIV);
	LITERAL1('%', T_PERCENT);
	LITERAL1('<', T_LT);
	LITERAL1('>', T_GT);
	LITERAL1('^', T_BW_XOR);
	LITERAL1('|', T_BW_OR);
	LITERAL1('?', T_TERNARY);
	
	if(c == '$') {
		SrcFile_count(f, c);
		
		while((c = SrcFile_getc(f)) != EOF && (IS_L(c) || IS_D(c)))
		{
			SrcFile_count(f, c);
			BUF_PUT(buf, c);
		}
		
		SrcFile_ungetc(f, c);
		BUF_PUT(buf, '\0');
		
		return Token_Placeholder(buf);
	}
	
	SrcFile_error(f, "Undefined token \"%c\"(%d)", c, (int) c);
	
	free(buf);
	
	return NULL;
	
	error_constchr_expectend:
		SrcFile_error(f, "Expecting ' in constant char");
		
		free(buf);
		return NULL;
	
	error_constchr_eof:
		SrcFile_error(f, "Unexpected EOF in constant char");
		
		free(buf);
		return NULL;
	
	error_conststr_eof:
		SrcFile_error(f, "Unexpected EOF in constant string");
		
		free(buf);
		return NULL;
}

#define case_return(name) case name: return #name;

const char *Token_getName(Token *t)
{
	if(t == NULL) {
		return "NULL";
	}
	
	switch(t->type)
	{
		case_return(T_WHITESPACE);
		case_return(T_MACRO);
		case_return(T_IDENTIFIER);
		case_return(T_HEXNUM);
		case_return(T_OCTNUM);
		case_return(T_DECNUM);
		case_return(T_CONSTSTR);
		case_return(T_CONSTCHR);
		case_return(T_ELLIPSIS);
		case_return(T_RIGHT_ASSIGN);
		case_return(T_LEFT_ASSIGN);
		case_return(T_ADD_ASSIGN);
		case_return(T_SUB_ASSIGN);
		case_return(T_MUL_ASSIGN);
		case_return(T_DIV_ASSIGN);
		case_return(T_MOD_ASSIGN);
		case_return(T_AND_ASSIGN);
		case_return(T_XOR_ASSIGN);
		case_return(T_OR_ASSIGN);
		case_return(T_RIGHT_OP);
		case_return(T_LEFT_OP);
		case_return(T_INC_OP);
		case_return(T_DEC_OP);
		case_return(T_PTR_OP);
		case_return(T_AND_OP);
		case_return(T_OR_OP);
		case_return(T_LE_OP);
		case_return(T_GE_OP);
		case_return(T_EQ_OP);
		case_return(T_NE_OP);
		case_return(T_SEMICOLON);
		case_return(T_BLOCKSTART);
		case_return(T_BLOCKEND);
		case_return(T_COMMA);
		case_return(T_COLON);
		case_return(T_EQUALS);
		case_return(T_LPAREN);
		case_return(T_RPAREN);
		case_return(T_LBRACKET);
		case_return(T_RBRACKET);
		case_return(T_DOT);
		case_return(T_BW_AND);
		case_return(T_NOT);
		case_return(T_BW_NOT);
		case_return(T_MINUS);
		case_return(T_PLUS);
		case_return(T_MUL);
		case_return(T_DIV);
		case_return(T_PERCENT);
		case_return(T_LT);
		case_return(T_GT);
		case_return(T_BW_XOR);
		case_return(T_BW_OR);
		case_return(T_TERNARY);
		case_return(T_PLACEHOLDER);
		case_return(T_EOF);
	default:
		return "UNKNOWN";
	}
}

#define case_return_conststr(name, string)           \
	case name:                                       \
		str = malloc(sizeof(char) * sizeof(string)); \
		strncpy(str, string, sizeof(string));        \
		return str;

#define case_return_str(name, string)          \
	case name: { int slen = strlen(string);    \
		str = malloc(sizeof(char) * slen + 1); \
		memcpy(str, string, slen + 1);         \
		return str; }

/**
 * Returns the string representation of the supplied token.
 * The returned string IS a copy, and needs to be freed manually.
 */
char *Token_getAsString(Token *tok)
{
	char *str;
	
	if(tok == NULL) {
		str = malloc(sizeof(char));
		str[0] = '\0';
		return str;
	}
	
	switch(tok->type) {
		case T_PLACEHOLDER:
		{
			int slen = strlen(tok->data);
			str = malloc(sizeof(char) * slen + 2);
			memcpy(str + 1, tok->data, slen + 1);
			str[0] = '$';
			return str;
		}
		case_return_str(T_WHITESPACE, tok->data);
		case_return_str(T_MACRO, tok->data);
		case_return_str(T_IDENTIFIER, tok->data);
		case T_HEXNUM:
		{
			int slen = strlen(tok->data);
			str = malloc(sizeof(char) * slen + 3);
			memcpy(str + 2, tok->data, slen + 1);
			str[0] = '0'; str[1] = 'x';
			return str;
		}
		case T_OCTNUM:
		{
			int slen = strlen(tok->data);
			str = malloc(sizeof(char) * slen + 2);
			memcpy(str + 1, tok->data, slen + 1);
			str[0] = '0';
			return str;
		}
		case_return_str(T_DECNUM, tok->data);
		case T_CONSTSTR:
		{
			int slen = strlen(tok->data);
			str = malloc(sizeof(char) * slen + 3);
			memcpy(str + 1, tok->data, slen);
			str[0] = '"'; str[slen + 1] = '"'; str[slen + 2] = '\0';
			return str;
		}
		case T_CONSTCHR:
		{
			int slen = strlen(tok->data);
			str = malloc(sizeof(char) * slen + 3);
			memcpy(str + 1, tok->data, slen);
			str[0] = '\''; str[slen] = '\''; str[slen + 1] = '\0';
			return str;
		}
		case_return_conststr(T_ELLIPSIS, "...");
		case_return_conststr(T_RIGHT_ASSIGN, ">>=");
		case_return_conststr(T_LEFT_ASSIGN, "<<=");
		case_return_conststr(T_ADD_ASSIGN, "+=");
		case_return_conststr(T_SUB_ASSIGN, "-=");
		case_return_conststr(T_MUL_ASSIGN, "*=");
		case_return_conststr(T_DIV_ASSIGN, "/=");
		case_return_conststr(T_MOD_ASSIGN, "%=");
		case_return_conststr(T_AND_ASSIGN, "&=");
		case_return_conststr(T_XOR_ASSIGN, "^=");
		case_return_conststr(T_OR_ASSIGN, "|=");
		case_return_conststr(T_RIGHT_OP, ">>");
		case_return_conststr(T_LEFT_OP, "<<");
		case_return_conststr(T_INC_OP, "++");
		case_return_conststr(T_DEC_OP, "--");
		case_return_conststr(T_PTR_OP, "->");
		case_return_conststr(T_AND_OP, "&&");
		case_return_conststr(T_OR_OP, "||");
		case_return_conststr(T_LE_OP, "<=");
		case_return_conststr(T_GE_OP, ">=");
		case_return_conststr(T_EQ_OP, "==");
		case_return_conststr(T_NE_OP, "!=");
		case_return_conststr(T_SEMICOLON, ";");
		case_return_conststr(T_BLOCKSTART, "{");
		case_return_conststr(T_BLOCKEND, "}");
		case_return_conststr(T_COMMA, ",");
		case_return_conststr(T_COLON, ":");
		case_return_conststr(T_EQUALS, "=");
		case_return_conststr(T_LPAREN, "(");
		case_return_conststr(T_RPAREN, ")");
		case_return_conststr(T_LBRACKET, "[");
		case_return_conststr(T_RBRACKET, "]");
		case_return_conststr(T_DOT, ".");
		case_return_conststr(T_BW_AND, "&");
		case_return_conststr(T_NOT, "!");
		case_return_conststr(T_BW_NOT, "~");
		case_return_conststr(T_MINUS, "-");
		case_return_conststr(T_PLUS, "+");
		case_return_conststr(T_MUL, "*");
		case_return_conststr(T_DIV, "/");
		case_return_conststr(T_PERCENT, "%");
		case_return_conststr(T_LT, "<");
		case_return_conststr(T_GT, ">");
		case_return_conststr(T_BW_XOR, "^");
		case_return_conststr(T_BW_OR, "|");
		case_return_conststr(T_TERNARY, "?");
		case_return_conststr(T_EOF, "");
		default:
			str = malloc(sizeof(char));
			str[0] = '\0';
			return str;
	}
}











