#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "Opcode.h"
#include "SrcFile.h"
#include "Token.h"

#define OPCODE_PREFIX "INSTR_"
#define OPCODE_H_GUARD_DEFINE "INSTRUCTION_OPCODE_H"
#define OPCODE_2_STR_FUNCNAME "Instruction_getTypeName"

/**
 * Replaces the placeholder token with either one or more tokens representing the
 * finished C code.
 * 
 * (*placeholder) should be Token_free()ed before a successful return.
 * **list_end is a pointer to the end of the current token list, as it is
 * building, append the replacement tokens to this list.
 */
int expandPlaceholder(Opcode *op, Token **placeholder, Token **list_end, SrcFile *fp)
{
	Token *replacement;
	const char *c;
	char *end;
	long l;
	
	c = (*placeholder)->data;
	
	/* Parameter placeholder */
	if(IS_D(*c)) {
		l = strtol(c, &end, 10);
		if(errno != 0 || *end != '\0') {
			errno = 0;
			goto error_param_placeholder;
		}
		
		if(l < 1 || l > op->num_params) {
			goto error_param_placeholder;
		}
		
		assert(l < 10);
		
		char *pos;
		char *data;
		
		switch(op->params[l - 1])
		{
			case PARAM_VARIABLE:
			{
				const char *template = "VM_VAR(PARAM@().asU)";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				pos = strchr(data, '@');
				*pos = '0' + l;
				replacement = Token_Identifier(data);
				break;
			}
			case PARAM_CFRAMEID:
			{
				const char *template = "frame->functions[PARAM@().asU]";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				pos = strchr(data, '@');
				*pos = '0' + l;
				replacement = Token_Identifier(data);
				break;
			}
			case PARAM_INTEGER:
			{
				const char *template = "PARAM@().asI";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				pos = strchr(data, '@');
				*pos = '0' + l;
				replacement = Token_Identifier(data);
				break;
			}
			case PARAM_UINT:
			{
				const char *template = "PARAM@().asU";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				pos = strchr(data, '@');
				*pos = '0' + l;
				replacement = Token_Identifier(data);
				break;
			}
			/* Parameter constants */
			case PARAM_CONST_INT:
			{
				const char *template = "PARAMC().asI";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				replacement = Token_Identifier(data);
				break;
			}
			case PARAM_CONST_UINT:
			{
				const char *template = "PARAMC().asU";
				data = malloc(strlen(template) + 1);
				memcpy(data, template, strlen(template) + 1);
				replacement = Token_Identifier(data);
				break;
			}
			default:
				goto error_param_type;
		}
		
		(*list_end)->next = replacement;
		(*list_end) = replacement;
		
		Token_free(*placeholder);
		
		return 1;
	}
	
	/* TODO: More placeholders? */
	
	
error_param_placeholder:
	
	if(op->num_params > 1) {
		SrcFile_error(fp, "Invalid parameter placeholder \"$%s\", must be positive integer "
			"between 1 and %d", (*placeholder)->data, op->num_params);
	} else if(op->num_params == 1) {
		SrcFile_error(fp, "Invalid parameter placeholder \"$%s\", must be $1 (only one parameter)",
			(*placeholder)->data);
	} else if(op->num_params == 0) {
		SrcFile_error(fp, "Invalid parameter placeholder \"$%s\", opcode does not have parameters",
			(*placeholder)->data);
	}
	
	return 0;
	
error_param_type:

	SrcFile_error(fp, "Parameter type for placeholder $%ld is invalid", l);
	
	return 0;
	
}

Opcode *parseOpcodeFile(const char *src)
{
	SrcFile *fp         = NULL;
	Opcode  *opcode     = NULL;
	Opcode  *firstop    = NULL;
	Token   *tok        = NULL;
	Token   *lastmacro  = NULL;
	Token   *firstmacro = NULL;
	Token   *prev       = NULL;
	char    *token_str  = NULL;
	int   op_param_idx  = 0;
	int   op_body_nest  = 0;
	Opcode_ParamType type = 0;
	
	fp = SrcFile_openFile(src);
	
	if( ! fp) {
		return NULL;
	}
	
find_op_def:
	while((tok = Token_nextToken(fp, 0)) != NULL)
	{
		if(tok->type == T_EOF) {
			Token_free(tok);
			goto parse_end;
		} else if(tok->type == T_MACRO) {
			if(lastmacro == NULL) {
				lastmacro = tok;
				firstmacro = lastmacro;
			} else {
				/* TODO: Remember macro and instruction sequence? */
				lastmacro->next = tok;
				lastmacro = lastmacro->next;
			}
		} else if(tok->type == T_IDENTIFIER && tok->data != NULL && strcmp(tok->data, "ins") == 0) {
			goto opcode_def;
		} else {
			goto unexpected_token;
		}
	}
	
opcode_def:
	
	Token_free(tok);
	/* Token name */
	tok = Token_nextToken(fp, 0);
	if(tok->type != T_IDENTIFIER) {
		goto unexpected_token;
	}
	/* Alloc new opcode */
	if(opcode == NULL) {
		opcode = Opcode_new();
		firstop = opcode;
	} else {
		opcode->next = Opcode_new();
		opcode = opcode->next;
	}
	
	opcode->name = Token_getAsString(tok);
	Token_free(tok);
	tok = Token_nextToken(fp, 0);
	if(tok->type != T_LPAREN) {
		goto unexpected_token;
	}
	Token_free(tok);
	
	op_param_idx = 0;
	
opcode_parameter:
	
	tok = Token_nextToken(fp, 0);
	
	if(tok->type == T_RPAREN) {
		/* We have a comma without a variable after it */
		if(op_param_idx != 0 && opcode->params[op_param_idx] == 0) {
			goto unexpected_token;
		}
		
		Token_free(tok);
		opcode->num_params = op_param_idx + 1;
		
		goto opcode_body;
	} else if(tok->type == T_COMMA) {
		if(opcode->params[op_param_idx] == 0) {
			goto unexpected_token;
		}
		
		Token_free(tok);
		op_param_idx++;
		goto opcode_parameter;
	} else if(tok->type != T_IDENTIFIER) {
		goto unexpected_token;
	}
	
	if(op_param_idx >= OPCODE_MAX_PARAMS) {
		goto unexpected_number_of_params;
	}
	
	/* To avoid "int uint" stuff overwriting previous stuff */
	if(opcode->params[op_param_idx] & ~PARAM_CONST) {
		goto unexpected_token;
	}
	
	type = Opcode_str2paramType(tok->data);
	
	if(type == PARAM_UNKNOWN) {
		goto unexpected_parameter_type;
	}
	
	if(type == PARAM_CONST && op_param_idx != 2) {
		goto unexpected_const_param;
	}
	
	opcode->params[op_param_idx] |= type;
	
	/* Const can only be applied to integer parameters */
	if(opcode->params[op_param_idx] != PARAM_CONST &&
		(opcode->params[op_param_idx] & PARAM_CONST) == PARAM_CONST &&
		((opcode->params[op_param_idx] & PARAM_INTEGER) != PARAM_INTEGER)) {
		goto unexpected_const_param_type;
	}
	
	Token_free(tok);
	goto opcode_parameter;
	
opcode_body:
	tok = Token_nextToken(fp, 0);
	if(tok->type != T_BLOCKSTART) {
		goto unexpected_token;
	}
	
	op_body_nest = 1;
	opcode->tokens = tok;
	prev = tok;
	
	while(op_body_nest > 0)
	{
		tok = Token_nextToken(fp, 1);
		
		if(tok->type == T_EOF) {
			goto unexpected_token;
		} else if(tok->type == T_BLOCKSTART) {
			op_body_nest++;
		} else if(tok->type == T_BLOCKEND) {
			op_body_nest--;
		} else if(tok->type == T_PLACEHOLDER) {
			/* Specific logic for the OpCompiler goes here */
			if(expandPlaceholder(opcode, &tok, &prev, fp) != 1) {
				Token_free(tok);
				goto free_opcodes_and_return;
			}
			continue;
		}
		
		prev->next = tok;
		prev = prev->next;
	}
	
	/* Done with this opcode */
	goto find_op_def;
	
unexpected_token:
	
	token_str = Token_getAsString(tok);
	SrcFile_error(fp, "Unexpected token %s: \"%s\"", Token_getName(tok), token_str);
	Token_free(tok);
	free(token_str);
	
	goto free_opcodes_and_return;
	
unexpected_parameter_type:
	
	SrcFile_error(fp, "Unexpected opcode parameter type \"%s\"", tok->data);
	Token_free(tok);
	
	goto free_opcodes_and_return;
	
unexpected_number_of_params:
	
	SrcFile_error(fp, "Too many parameters for opcode \"%s\"", opcode->name);
	Token_free(tok);
	
	goto free_opcodes_and_return;
	
unexpected_const_param:
	
	SrcFile_error(fp, "const modifier can only be used on the third parameter in \"%s\"",
		opcode->name);
	Token_free(tok);
	
	goto free_opcodes_and_return;
	
unexpected_const_param_type:

	token_str = Token_getAsString(tok);
	SrcFile_error(fp, "const modifier cannot be applied to the type %s, only int and uint"
		" are allowed, in \"%s\"",
		token_str, opcode->name);
	free(token_str);
	Token_free(tok);
	
	goto free_opcodes_and_return;

	
free_opcodes_and_return:
	/* Free the opcode linked list */
	opcode = firstop;
	while(opcode)
	{
		Opcode *next;
		
		for(tok = opcode->tokens; tok;)
		{
			Token *next = tok->next;
			Token_free(tok);
			tok = next;
		}
		
		next = opcode->next;
		free(opcode->name);
		free(opcode);
		opcode = next;
	}
	
	SrcFile_free(fp);
	
	return NULL;
	
	
parse_end:
	
	SrcFile_free(fp);
	
	return firstop;
}

int printStaticOpcodeDataToFile(const Opcode *opcodes, const char *filename, const char *srcfile)
{
	const Opcode *o;
	FILE *fd;
	unsigned int num = 0;
	
	printf("Writing opcode enum to %s...", filename);
	
	/* TODO: Check for write success? */
	
	fd = fopen(filename, "w");
	
	if(fd == NULL) {
		fprintf(stderr, "\nERROR: Could not open %s for writing.\n", filename);
		
		return 0;
	}
	
	/* TODO: Replace OPCODES_STATIC_H with something derived from filename */
	fprintf(fd, "/* File generated by " __FILE__ " from %s */\n\n"
"#ifndef "OPCODE_H_GUARD_DEFINE"\n"
"#define "OPCODE_H_GUARD_DEFINE"\n\n"
"#include <stdint.h>\n\n"
"/* Datatype for instruction opcodes */\n"
"typedef uint8_t Instruction_opcode;\n\n", srcfile);
	
	for(o = opcodes; o; o = o->next) {
		fprintf(fd, "#define " OPCODE_PREFIX "%s %d\n", o->name, num);
		num++;
	}
	
	fputs("\n\n/* Number of opcode parameters */\n", fd);
	
	for(o = opcodes; o; o = o->next) {
		fprintf(fd, "#define " OPCODE_PREFIX "%s_PARAMS %d\n", o->name, o->num_params);
		/* TODO: Add a define for if an opcode has a constant second parameter (32-bit) */
		num++;
	}
	
	fputs("\n\n/* Opcode \"Constructor\" macros */\n\n", fd);
	
	for(o = opcodes; o; o = o->next) {
		int i = 0;
		int n = 0;
		
		fprintf(fd, "/* " OPCODE_PREFIX "%s ", o->name);
		
		for(i = 0, n = 0; i < o->num_params; i++) {
			if(o->params[i] != PARAM_UNDEF && o->params[i] != PARAM_UNKNOWN && o->params[i] != 0) {
				if(n > 0) {
					fputs(", ", fd);
				}
				
				if((o->params[i] & PARAM_CONST) == PARAM_CONST) {
					fputs("32bit ", fd);
				}
				
				fputs(Opcode_paramType2str(o->params[i]), fd);
				
				n++;
			}
		}
		
		fputs(" */\n", fd);
		
		fprintf(fd, "#define " OPCODE_PREFIX "%s_NEW(", o->name);
		
		for(i = 0, n = 0; i < o->num_params; i++) {
			if(o->params[i] != PARAM_UNDEF && o->params[i] != PARAM_UNKNOWN && o->params[i] != 0) {
				if(n > 0) {
					fputs(", ", fd);
				}
				
				fputs(Opcode_paramType2str(o->params[i]), fd);
				assert(i >= 0);
				assert(i < 10);
				fputc('0' + i, fd);
				
				n++;
			}
		}
		
		fprintf(fd, ") (Instruction) {.type = " OPCODE_PREFIX "%s", o->name);
		
		for(i = 0, n = 0; i < o->num_params; i++) {
			if(o->params[i] != PARAM_UNDEF && o->params[i] != PARAM_UNKNOWN && o->params[i] != 0) {
				fputs(", ", fd);
				
				if(i == 0) {
					fputs(".p1", fd);
				} else if(i == 1) {
					fputs(".p2", fd);
				} else if(i == 2 && (o->params[i] & PARAM_CONST) == PARAM_CONST) {
					fputs(".u.c", fd);
				} else if(i == 2) {
					fputs(".u.i8.p3", fd);
				} else if(i == 3) {
					fputs(".u.i8.p4", fd);
				} else if(i == 4) {
					fputs(".u.i8.p5", fd);
				} else if(i == 5) {
					fputs(".u.i8.p6", fd);
				}
				
				switch(o->params[i] & ~PARAM_CONST) {
					case PARAM_INTEGER:
						fputs(".asI", fd);
						break;
					case PARAM_UINT:
					case PARAM_VARIABLE:
					case PARAM_CFRAMEID:
						fputs(".asU", fd);
						break;
				}
				
				fputs(" = ", fd);
				
				fputs(Opcode_paramType2str(o->params[i]), fd);
				assert(i >= 0);
				assert(i < 10);
				fputc('0' + i, fd);
				
				n++;
			}
		}
		
		fputs("}\n", fd);
		num++;
	}
	
	/* TODO: Is opcode parameter size needed here? or is it enough with
	         being hardcoded inside the generated VM switch-case? */
	
	printf("Done, %u opcodes written.\n", num);
	
	fputs("\n#endif", fd);
	fclose(fd);
	
	return 1;
}


int printOpcodeToStringCFile(const Opcode *opcodes, const char *filename, const char *srcfile)
{
	const Opcode *o;
	FILE *fd;
	unsigned int num = 0;
	
	printf("Writing opcode2str function to %s...", filename);
	
	/* TODO: Check for write success? */
	
	fd = fopen(filename, "w");
	
	if(fd == NULL) {
		fprintf(stderr, "\nERROR: Could not open %s for writing.\n", filename);
		
		return 0;
	}
	
	fprintf(fd, "/* File generated by " __FILE__ " from %s */\n\n"
		"#include \"Instruction.h\"\n"
		"\n"
		"char *"OPCODE_2_STR_FUNCNAME"(const Instruction *const instr)\n"
		"{\n"
		"\tswitch(instr->type)\n"
		"\t{\n", srcfile);
	
	for(o = opcodes; o; o = o->next) {
		fprintf(fd, "\t\tcase " OPCODE_PREFIX "%s: return \"" OPCODE_PREFIX "%s\";\n",
			o->name, o->name);
		num++;
	}
	
	fprintf(fd, "\t\tdefault: return \"UNKNOWN\";\n"
		"\t}\n"
		"}\n");
	
	printf("Done, %u opcodes written.\n", num);
	fclose(fd);
	
	return 1;
}


int printOpcodeLoopToFile(const Opcode *opcodes, const char *filename, const char *srcfile)
{
	const Opcode *o;
	FILE *fd;
	unsigned int num = 0;
	Token *tok;
	
	printf("Writing opcode switch-case body to %s...", filename);
	
	/* TODO: Check for write success? */
	
	fd = fopen(filename, "w");
	
	if(fd == NULL) {
		fprintf(stderr, "\nERROR: Could not open %s for writing.\n", filename);
		
		return 0;
	}
	
	fprintf(fd, "/* File generated by " __FILE__ " from %s */\n\n", srcfile);
	
	for(o = opcodes; o; o = o->next) {
		fprintf(fd, "case " OPCODE_PREFIX "%s:\n", o->name);
		
		for(tok = o->tokens; tok; tok = tok->next)
		{
			char *str = Token_getAsString(tok);
			fputs(str, fd);
			free(str);
		}
		
		fprintf(fd, "\nINSTR_NEXT();\nbreak;\n\n");
		num++;
	}
	
	printf("Done, %u opcodes written.\n", num);
	fclose(fd);
	
	return 1;
}


int main (int argc, char const *argv[])
{
	if(argc < 5)
	{
		printf("Usage: [source.def] [opcode_defines.h] [opcode2str_out.c]"
			" [eval_body_out.inc]\n");
		return -1;
	}
	
	Token *tok = NULL;
	Opcode *op = NULL;
	Opcode *opcodes = parseOpcodeFile(argv[1]);
	
	if(opcodes == NULL) {
		exit(-1);
	}
	
	if( ! printStaticOpcodeDataToFile(opcodes, argv[2], argv[1])) {
		exit(-1);
	}
	
	if( ! printOpcodeToStringCFile(opcodes, argv[3], argv[1])) {
		exit(-1);
	}
	
	if( ! printOpcodeLoopToFile(opcodes, argv[4], argv[1])) {
		exit(-1);
	}
	
	/* Free opcodes */
	for(op = opcodes; op;)
	{
		Opcode *next;
		for(tok = op->tokens; tok;)
		{
			Token *next = tok->next;
			Token_free(tok);
			tok = next;
		}
		
		next = op->next;
		free(op->name);
		free(op);
		op = next;
	}

#ifdef LEAKCHECK_H
	LeakCheck_printMemReport(1);
#endif
	
	return 0;
}
