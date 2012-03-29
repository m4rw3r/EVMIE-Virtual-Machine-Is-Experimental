
/* sprintf */
#include <stdio.h>
#include <stdlib.h>

#include "Value.h"
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
	Frame *frame = malloc(sizeof(Frame));
	Frame_init(frame);
	
	uint64_t varsum = Frame_allocVariable(frame);
	uint64_t varn   = Frame_allocVariable(frame);
	uint64_t vara   = Frame_allocVariable(frame);
	uint64_t varb   = Frame_allocVariable(frame);
	uint64_t vari   = Frame_allocVariable(frame);
	
	Instruction instructions[] = {
		{INS(SETTO),  {.asUInt = varsum}, {.asValue = Value_fromInt32(0) }, NULL},
		{INS(SETTO),  {.asUInt = varn},   {.asValue = Value_fromInt32(20)}, NULL},
		{INS(SETTO),  {.asUInt = vara},   {.asValue = Value_fromInt32(0) }, NULL},
		{INS(SETTO),  {.asUInt = varb},   {.asValue = Value_fromInt32(1) }, NULL},
		{INS(SETTO),  {.asUInt = vari},   {.asValue = Value_fromInt32(0) }, NULL},
		{INS(IS_LT),  {.asUInt = vari},   {.asUInt  = varn},                NULL},
		{INS(JMPZ),   {.asInt  = 9},      {.asInt   = 0},                   NULL},
		{INS(VARADD), {.asUInt = vari},   {.asInt   = 1},                   NULL},
		{INS(ADD),    {.asUInt = vara},   {.asUInt  = varb},                NULL},
		{INS(SETLV),  {.asUInt = varsum}, {.asInt   = 0},                   NULL},
		{INS(COPY),   {.asUInt = varb},   {.asUInt  = vara},                NULL},
		{INS(COPY),   {.asUInt = varsum}, {.asUInt  = varb},                NULL},
		{INS(FETCH),  {.asUInt = varsum}, {.asInt   = 0},                   NULL},
		{INS(PRINT),  {.asInt  = 0},      {.asInt   = 0},                   NULL},
		{INS(JMP),    {.asInt  = -9},     {.asInt   = 0},                   NULL},
		{INS(FETCH),  {.asUInt = varsum}, {.asInt   = 0},                   NULL},
		{INS(PRINT),  {.asInt  = 0},      {.asInt   = 0},                   NULL},
	};
	
	int j;
	for(j = 0; j < 10; j++)
	{
		printf("int %s\n", byte_to_binary((uint64_t) malloc(sizeof(int))));
	}
	
	int number = (sizeof(instructions) / sizeof(Instruction)) - 1;
	
	for(; number > 0; number--)
	{
		instructions[number - 1].next = &instructions[number];
	}
	
	frame->instructions = instructions;
	
	Frame_packFrame(frame);
	
	printf("Num instructions %d\n", frame->num_instructions);
	
	unsigned int i;
	for(i = 0; i < frame->num_instructions ; i++)
	{
		printf("Instruction %2d: %-11s, %016llx\n", i, Instruction_getTypeName(&frame->instructions[i]), (uint64_t) &frame->instructions[i]);
	}
	
	Eval_execFrame(frame);
	
	return 0;
}
