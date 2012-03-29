
/* sprintf */
#include <stdio.h>
#include <stdlib.h>

#include "Value.h"
#include "Instruction.h"
#include "Eval.h"
#include "Frame.h"

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
	/* Function to calcuate the 20 first fibonacci numbers */
	Frame *frame = malloc(sizeof(Frame));
	Frame_init(frame);
	
	uint64_t varsum = Frame_allocVariable(frame);
	uint64_t varn   = Frame_allocVariable(frame);
	uint64_t vara   = Frame_allocVariable(frame);
	uint64_t varb   = Frame_allocVariable(frame);
	uint64_t vari   = Frame_allocVariable(frame);
	
	Instruction instructions[] = {
		{INS(SETTO),  {.asUInt = varsum}, {.asValue = Value_fromInt32(0) }},
		{INS(SETTO),  {.asUInt = varn},   {.asValue = Value_fromInt32(20)}},
		{INS(SETTO),  {.asUInt = vara},   {.asValue = Value_fromInt32(0) }},
		{INS(SETTO),  {.asUInt = varb},   {.asValue = Value_fromInt32(1) }},
		{INS(SETTO),  {.asUInt = vari},   {.asValue = Value_fromInt32(0) }},
		{INS(IS_LT),  {.asUInt = vari},   {.asUInt  = varn}},
		{INS(JMPZ),   {.asInt  = 9},      {.asInt   = 0}},
		{INS(VARADD), {.asUInt = vari},   {.asInt   = 1}},
		{INS(ADD),    {.asUInt = vara},   {.asUInt  = varb}},
		{INS(SETLV),  {.asUInt = varsum}, {.asInt   = 0}},
		{INS(COPY),   {.asUInt = varb},   {.asUInt  = vara}},
		{INS(COPY),   {.asUInt = varsum}, {.asUInt  = varb}},
		{INS(FETCH),  {.asUInt = varsum}, {.asInt   = 0}},
		{INS(PRINT),  {.asInt  = 0},      {.asInt   = 0}},
		{INS(JMP),    {.asInt  = -9},     {.asInt   = 0}},
		{INS(RET), {.asInt = 0}, {.asInt = 0}},
	};
	
	int number = (sizeof(instructions) / sizeof(Instruction));
	
	Frame_appendInstructions(frame, instructions, number);
	
	/* Function to call it 10 times */
	Frame *frame_main = malloc(sizeof(Frame));
	Frame_init(frame_main);
	uint64_t varloop = Frame_allocVariable(frame_main);
	uint64_t varlc = Frame_allocVariable(frame_main);
	
	Instruction instructions_main[] = {
		{INS(SETTO), {.asUInt = varloop}, {.asValue = Value_fromInt32(10)}},
		{INS(SETTO), {.asUInt = varlc}, {.asValue = Value_fromInt32(0)}},
		{INS(IS_LT), {.asUInt = varlc}, {.asUInt = varloop}},
		{INS(JMPZ),  {.asInt  = 4}, {.asUInt = 0}},
		{INS(VARADD),{.asUInt = varlc},   {.asInt   = 1}},
		{INS(CALL),  {.asUInt = Frame_allocFunction(frame_main, frame)}, {.asUInt = 0}},
		{INS(JMP),   {.asInt  = -4}, {.asInt = 0}},
	};
	
	Frame_appendInstructions(frame_main, instructions_main, sizeof(instructions_main) / sizeof(Instruction));
	
	/* Compile the stuff */
	Frame_compileFrame(frame_main);
	
	printf("Fibonacci instructions:\n");
	unsigned int i;
	for(i = 0; i < ARRAY_SIZE(frame->instructions); i++)
	{
		printf("Instruction %2d: %-11s, %016llx\n", i, Instruction_getTypeName(&frame->instructions[i]), (uint64_t) &frame->instructions[i]);
	}
	
	printf("Calling instructions:\n");
	for(i = 0; i < ARRAY_SIZE(frame_main->instructions); i++)
	{
		printf("Instruction %2d: %-11s, %016llx\n", i, Instruction_getTypeName(&frame_main->instructions[i]), (uint64_t) &frame_main->instructions[i]);
	}
	
	/* Run */
	Eval_execFrame(frame_main);
	
	return 0;
}
