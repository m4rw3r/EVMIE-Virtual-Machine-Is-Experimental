#ifndef OPCODE_H
#define OPCODE_H

#include <string.h>
#include <stdlib.h>
#include "Token.h"

/* The maximum explicitly defined parameters an opcode can have,
   the Instruction struct decides the number of possible parameters */
#define OPCODE_MAX_PARAMS 6

/* Int and uint is type & PARAM_INTEGER == PARAM_INTEGER so it is easy to check for plain parameters */
typedef enum Opcode_ParamType
{
	PARAM_CONST      = 1,  /* Constant 32-bit third parameter, modifier,
	                          prevents existance of 3-6th parameters, only works on int and uint */
	PARAM_VARIABLE   = 2,  /* uint8_t, Variable fetched from supplied CFrame
	                          with internal type uint16_t */
	
	PARAM_INTEGER    = 4,  /* signed 8 bit int constant */
	PARAM_UINT       = 12, /* unsigned 8 bit int  (4 (INT) + 8 = UINT) */
	PARAM_CFRAMEID   = 16, /* "Function" index in CFrame->functions (uint8_t) */
	
	PARAM_UNKNOWN    = 32, /* Not recognized parameter type, raise error */
	PARAM_UNDEF      = 64, /* Parameter not used */
	
	PARAM_CONST_INT  = 5,  /* 4 + 1 */
	PARAM_CONST_UINT = 13, /* 13 + 1 */
	
	PARAM_VARINT     = 6,  /* 4 + 2     */
	PARAM_VARUINT    = 14, /* 4 + 8 + 2 */
	PARAM_VARCFRAME  = 18, /* 16 + 2    */
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

static inline Opcode *Opcode_new()
{
	Opcode *opcode = malloc(sizeof(Opcode));
	memset(opcode, 0, sizeof(Opcode));
	return opcode;
}

Opcode_ParamType Opcode_str2paramType(const char *str);
const char *Opcode_paramType2str(Opcode_ParamType type);

#endif