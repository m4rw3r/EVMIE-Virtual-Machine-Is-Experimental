
/* sprintf */
#include <stdio.h>
#include <stdlib.h>

#include "Value.h"
#include "Context.h"
#include "Instruction.h"
#include "Eval.h"

const char *byte_to_binary(uint64_t x)
{
    static char b[65];
    b[0] = '\0';

    uint64_t z;
    for(z = 64; z > 0; z--)
    {
        strcat(b, ((x & ((uint64_t)1 << z)) == ((uint64_t)1 << z)) ? "1" : "0");
    }

    return b;
}

int main()
{
	Context *ctx = malloc(sizeof(Context));
	Context_init(ctx);
	
	uint64_t varsum = Context_allocVariable(ctx);
	uint64_t varn   = Context_allocVariable(ctx);
	uint64_t vara   = Context_allocVariable(ctx);
	uint64_t varb   = Context_allocVariable(ctx);
	uint64_t vari   = Context_allocVariable(ctx);
	
	Instruction instructions[] = {
		{INS(SETTO),  varsum, Value_fromInt32(0).asBits,  NULL},
		{INS(SETTO),  varn,   Value_fromInt32(20).asBits, NULL},
		{INS(SETTO),  vara,   Value_fromInt32(0).asBits,  NULL},
		{INS(SETTO),  varb,   Value_fromInt32(1).asBits,  NULL},
		{INS(SETTO),  vari,   Value_fromInt32(0).asBits,  NULL},
		{INS(IS_LT),  vari,   varn,                       NULL},
		{INS(JMPZ),   0,      0,                          NULL},
		{INS(VARADD), vari,   1,                          NULL},
		{INS(ADD),    vara,   varb,                       NULL},
		{INS(SETLV),  varsum, 0,                          NULL},
		{INS(COPY),   varb,   vara,                       NULL},
		{INS(COPY),   varsum, varb,                       NULL},
		{INS(FETCH),  varsum, 0,                          NULL},
		{INS(PRINT),  0,      0,                          NULL},
		{INS(JMP),    0,      0,                          NULL},
		{INS(FETCH),  varsum, 0,                          NULL},
		{INS(PRINT),  0,      0,                          NULL},
	};
	
	int j;
	for(j = 0; j < 10; j++)
	{
		printf("int %s\n", byte_to_binary((uint64_t) malloc(sizeof(int))));
	}
	
	int number = sizeof(instructions) / sizeof(Instruction);
	
	int i;
	for(i = 0; i < number; i++)
	{
		printf("Instruction %2d: %-11s, %016llx\n", i, Instruction_getTypeName(&instructions[i]), (uint64_t) &instructions[i]);
	}
	
	for(i = 1; i < number; i++)
	{
		instructions[i - 1].next = &instructions[i];
	}
	
	instructions[6].data1  = (uint64_t) &instructions[15];
	instructions[14].data1 = (uint64_t) &instructions[5];
	
	Eval_execInstructions(instructions, ctx);
	
	return 0;
}
