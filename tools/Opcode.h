#ifndef OPCODE_H
#define OPCODE_H

#include <string.h>
#include <stdlib.h>
#include "Token.h"

/* The maximum explicitly defined parameters an opcode can have */
#define OPCODE_MAX_PARAMS 3

typedef enum Opcode_ParamType
{
	PARAM_VARIABLE,  /* uint32_t, Variable fetched from supplied CFrame index */
	PARAM_INTEGER,   /* signed 64 bit int */
	PARAM_UINT,      /* unsigned 64 bit int */
	PARAM_VALUE,     /* Value from Value.h */
	PARAM_CFRAMEIDX, /* "Function" index in CFrame->functions */
	PARAM_UNKNOWN,   /* Not recognized parameter type, raise error */
} Opcode_ParamType;

struct Opcode;

typedef struct Opcode
{
	char *name;
	Opcode_ParamType params[OPCODE_MAX_PARAMS];
	short num_params;
	struct Opcode *next;
	Token *tokens;
} Opcode;

inline Opcode *Opcode_new()
{
	Opcode *opcode = malloc(sizeof(Opcode));
	memset(opcode, 0, sizeof(Opcode));
	return opcode;
}

Opcode_ParamType Opcode_str2paramType(const char *str);

#endif