#ifndef INSTRUCTION_H
#define INSTRUCTION_H 1

#include <stdint.h>
#include "Instruction_opcode.gen.h"

typedef union Instruction_param {
	uint8_t asU;
	int8_t  asI;
} Instruction_param;

/* Should be 64-bit in total */
typedef struct Instruction {
	union {
		/* Constant parameters can be 32 bit, if instruction only has two parameters */
		union {
			uint32_t asU;
			int32_t  asI;
		} c;
		struct {
			Instruction_param p3;
			Instruction_param p4;
			Instruction_param p5;
			Instruction_param p6;
		} i8;
	} u;
	/* 8 bit space, what to do with it? */
	Instruction_param p2;
	Instruction_param p1;
	Instruction_opcode type;
} Instruction;

char *Instruction_getTypeName(const Instruction *const instr);

#endif