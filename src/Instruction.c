#include <assert.h>
#include "Instruction.h"
#include <stdio.h>
#include <stdint.h>

#define return_typename(name) case name: return #name;

char *Instruction_getTypeName(const Instruction *const instr)
{
	switch(instr->type)
	{
		return_typename(INS(NOP))
		return_typename(INS(ADD))
		return_typename(INS(SUB))
		return_typename(INS(MUL))
		return_typename(INS(DIV))
		return_typename(INS(MOD))
		return_typename(INS(SL))
		return_typename(INS(SR))
		return_typename(INS(BW_OR))
		return_typename(INS(BW_AND))
		return_typename(INS(BW_XOR))
		return_typename(INS(BW_NOT))
		return_typename(INS(IS_EQ))
		return_typename(INS(IS_LT))
		return_typename(INS(VARADD))
		return_typename(INS(SETLV))
		return_typename(INS(SETTO))
		return_typename(INS(EQ))
		return_typename(INS(JMP))
		return_typename(INS(JMPZ))
		return_typename(INS(JNZ))
		return_typename(INS(FETCH))
		return_typename(INS(COPY))
		return_typename(INS(PRINT))
		default:
			return "UNKNOWN";
	}
}

#undef return_typename(name)
