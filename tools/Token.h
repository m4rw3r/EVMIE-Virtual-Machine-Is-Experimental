#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include "SrcFile.h"

/* [A-Za-z_] */
#define IS_L(c) ((c > 64 && c < 91) || (c > 96 && c < 123) || c == 95)
/* [0-9] */
#define IS_D(c) (c > 47 && c < 58)
/* [a-fA-F0-9] */
#define IS_H(c) ((c > 96 && c < 103) || (c > 64 && c < 71) || IS_D(c))
/* whitespace */
#define IS_W(c) (c > 0 && c < 33)

/**
 * List of C tokens, but also includes:
 * T_WHITESPACE for when Token_nextToken() is allowed to return whitespace,
 * T_MACRO for C preprocessor macros (starting with #),
 * T_PLACEHOLDER for identifiers starting with $
 */
enum Token_type
{
	T_WHITESPACE,
	T_MACRO,
	T_IDENTIFIER,
	T_HEXNUM,
	T_OCTNUM,
	T_DECNUM,
	T_CONSTSTR,
	T_CONSTCHR,
	T_ELLIPSIS,
	T_RIGHT_ASSIGN,
	T_LEFT_ASSIGN,
	T_ADD_ASSIGN,
	T_SUB_ASSIGN,
	T_MUL_ASSIGN,
	T_DIV_ASSIGN,
	T_MOD_ASSIGN,
	T_AND_ASSIGN,
	T_XOR_ASSIGN,
	T_OR_ASSIGN,
	T_RIGHT_OP,
	T_LEFT_OP,
	T_INC_OP,
	T_DEC_OP,
	T_PTR_OP,
	T_AND_OP,
	T_OR_OP,
	T_LE_OP,
	T_GE_OP,
	T_EQ_OP,
	T_NE_OP,
	T_SEMICOLON,
	T_BLOCKSTART,
	T_BLOCKEND,
	T_COMMA,
	T_COLON,
	T_EQUALS,
	T_LPAREN,
	T_RPAREN,
	T_LBRACKET,
	T_RBRACKET,
	T_DOT,
	T_BW_AND,
	T_NOT,
	T_BW_NOT,
	T_MINUS,
	T_PLUS,
	T_MUL,
	T_DIV,
	T_PERCENT,
	T_LT,
	T_GT,
	T_BW_XOR,
	T_BW_OR,
	T_TERNARY,
	T_PLACEHOLDER,
	T_EOF,
};

struct Token;

typedef struct Token
{
	enum Token_type type;
	const char *data;
	struct Token *next;  /* Not used by the Token_nextToken() */
} Token;

static inline void Token_free(Token *t)
{
	free((char *)t->data);
	free(t);
}

#define TOKEN_CTOR(name, ttype) \
static inline Token *Token_ ## name (const char *data) { \
	Token *t = malloc(sizeof(Token));                    \
	t->type = ttype; t->data = data; t->next = NULL;     \
	return t; }

TOKEN_CTOR(Whitespace, T_WHITESPACE)
TOKEN_CTOR(Macro, T_MACRO)
TOKEN_CTOR(Identifier, T_IDENTIFIER)
TOKEN_CTOR(HexNum, T_HEXNUM)
TOKEN_CTOR(OctNum, T_OCTNUM)
TOKEN_CTOR(DecNum, T_DECNUM)
TOKEN_CTOR(ConstStr, T_CONSTSTR)
TOKEN_CTOR(ConstChr, T_CONSTCHR)
TOKEN_CTOR(Placeholder, T_PLACEHOLDER)

#undef TOKEN_CTOR(name, ttype)

static inline Token *Token_fromType(enum Token_type type)
{
	Token *t = malloc(sizeof(Token));
	t->type = type;
	t->data = NULL;
	t->next = NULL;
	
	return t;
}

Token *Token_nextToken(SrcFile *f, int return_whitespace);

const char *Token_getName(Token *t);

char *Token_getAsString(Token *tok);

#endif