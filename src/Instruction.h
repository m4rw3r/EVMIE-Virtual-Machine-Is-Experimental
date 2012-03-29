#ifndef INSTRUCTION_H
#define INSTRUCTION_H 1

#include <stdint.h>

#include "Value.h"

#define INS(x) INSTR_##x

enum Instruction_type
{
	INS(NOP),    /* NOOP */
	INS(ADD),    /* Add */
	INS(SUB),    /* Subtract */
	INS(MUL),    /* Multiply */
	INS(DIV),    /* Divide */
	INS(MOD),    /* Modulo */
	INS(SL),     /* Shift Left */
	INS(SR),     /* Shift Right */
	INS(BW_OR),  /* Bitwise OR */
	INS(BW_AND), /* Bitwise AND */
	INS(BW_XOR), /* Bitwise XOR */
	INS(BW_NOT), /* Bitwise NOT */
	INS(VARADD), /* Adds data2 to var(data1) */
	INS(IS_EQ),  /* Is equals (var(data1) == var(data2)) */
	INS(IS_LT),  /* Less than (var(data1) < var(data2)) */
	INS(SETLV),  /* Sets the varialble to last_value */
	INS(SETTO),  /* Sets variable with id data1 to data2 */
	INS(EQ),     /* Equals */
	INS(JMP),    /* Jump */
	INS(JMPZ),   /* Jump to data1 as pointer to Instruction if last_value == 0 */
	INS(JNZ),    /* Jump not zero */
	INS(FETCH),  /* Fetches the variable var(data1) to last_value */
	INS(COPY),   /* Copies the value from var(data1) to var(data2) */
	INS(PRINT),  /* Prints last_value */
	INS(CALL),
	INS(RET),
};

struct Instruction;

typedef struct Instruction {
	enum Instruction_type type;
	union {
		uint64_t asUInt;
		int64_t asInt;
		Value asValue;
	} data1;
	union {
		uint64_t asUInt;
		int64_t asInt;
		Value asValue;
	} data2;
} Instruction;

char *Instruction_getTypeName(const Instruction *const instr);

#endif