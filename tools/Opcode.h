#ifndef OPCODE_H
#define OPCODE_H

#include <string.h>
#include <stdlib.h>
#include "Token.h"

/* The maximum explicitly defined parameters an opcode can have */
#define OPCODE_MAX_PARAMS 6

/* Int and uint is type & PARAM_INTEGER == PARAM_INTEGER so it is easy to check for plain parameters */
typedef enum Opcode_ParamType
{
	PARAM_CONST    = 1,  /* Constant 32-bit third parameter, modifier,
	                        prevents existance of 4-6th parameters, only works on int and uint */
	PARAM_INTEGER  = 2,  /* signed 16 bit int constant */
	PARAM_CONST_INT = 3,
	PARAM_VARIABLE = 4,  /* uint64_t, Variable fetched from supplied CFrame
	                        with internal type uint16_t */
	PARAM_UINT     = 6,  /* unsigned 16 bit int */
	PARAM_CONST_UINT = 7,
	PARAM_CFRAMEID = 8,  /* "Function" index in CFrame->functions (uint16_t) */
	PARAM_UNKNOWN  = 16, /* Not recognized parameter type, raise error */
	PARAM_UNDEF    = 32, /* Parameter not used */
} Opcode_ParamType;

struct Opcode;

typedef struct Opcode
{
	char *name;
	Opcode_ParamType params[OPCODE_MAX_PARAMS];
	short num_params;
	short has_constant;
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
const char *Opcode_paramType2str(Opcode_ParamType type);

#endif