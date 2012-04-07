
#include <assert.h>

#include "Eval.h"
#include "Error.h"

#define VM_DEBUG_LEVEL 0

CFrame_Register Eval_execFrame(Frame *srcframe)
{
	register CFrame_Register *values;
	register const Instruction *instr;
	register const Instruction *last_instr;
	
	if( ! srcframe->compiled_frame)
	{
		Error_fatalError("ERROR: Frame 0x%016lx is not compiled!\n", (uintptr_t) srcframe);
	}
	
	register CFrame *frame = CFrame_alloc();
	CFrame_copy(frame, srcframe->compiled_frame);
	
	next_frame:
	
#if VM_DEBUG_LEVEL > 1
	printf("Preparing new CFrame, num_instructions: %ld\n",
		frame->last_instr - frame->cur_instr + 1);
#endif
	
	values     = frame->variables;
	instr      = frame->cur_instr;
	last_instr = frame->last_instr;
	
	while(instr <= last_instr)
	{
#if VM_DEBUG_LEVEL > 1
		printf("Running %s, %016llx\n", Instruction_getTypeName(instr), (uint64_t) instr);
#endif
		
		/* TODO: More instructions, and expand existing if needed */
		switch(instr->type)
		{

#define VM_VAR1     VM_VAR(PARAM(1).asUInt)
#define VM_VAR2     VM_VAR(PARAM(2).asUInt)
#define VM_VAR(var) values[(var)]
#define VM_VAL      last_value
#define PARAM1()    instr->p1
#define PARAM2()    instr->p2
#define PARAM3()    instr->u.i8.p3
#define PARAM4()    instr->u.i8.p4
#define PARAM5()    instr->u.i8.p5
#define PARAM6()    instr->u.i8.p6
#define PARAMC()    instr->u.c
#define INSTR_NEXT(num_params) instr++
#define VM_NOTIMPLEMENTEDOP() \
	fprintf(stderr, "Unhandled opcode %02x %s\n", instr->type, Instruction_getTypeName(instr));

#include "Eval_opcodes.gen.inc"

		}
	}
	
	end_eval:
	
	CFrame_free(frame);
	
#if VM_DEBUG_LEVEL > 1
	printf("Exiting Eval_execCFrame()\n");
#endif
	
	/* TODO: Return, how? */
	return (CFrame_Register) {.asU = 0};
}

#undef ArithmeticOp(operation)
