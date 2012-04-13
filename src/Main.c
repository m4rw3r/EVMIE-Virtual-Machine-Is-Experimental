
/* sprintf */
#include <stdio.h>
#include <stdlib.h>

#include "Instruction.h"
#include "Eval.h"
#include "Frame.h"

int main()
{
	/* Function to calcuate the n first fibonacci numbers */
	Frame *fib_frame = malloc(sizeof(Frame));
	Frame_init(fib_frame);
	
	uint32_t prevfr = Frame_allocVariable(fib_frame);
	/* No continuations called here, so we can get away with not allocating space for return values */
	uint32_t varsum = Frame_allocVariable(fib_frame);
	uint32_t varn   = Frame_allocVariable(fib_frame);
	uint32_t vara   = Frame_allocVariable(fib_frame);
	uint32_t varb   = Frame_allocVariable(fib_frame);
	uint32_t vari   = Frame_allocVariable(fib_frame);
	uint32_t vartmp = Frame_allocVariable(fib_frame);
	
	/* Set default value, if not set defaults to 0 */
	Frame_setVariable(fib_frame, varn, (CFrame_Register) {.asU = 20});
	/* Initial value for variable */
	Frame_setVariable(fib_frame, varb, (CFrame_Register) {.asU = 1});
	
	Instruction instrs[] = {
		INSTR_SUB_NEW(varn, vari, vartmp),  /* tmp = n - i; */
		INSTR_BLTZ_NEW(vartmp, 6),          /* if( ! (tmp < 0)) { */
		INSTR_ADDI_NEW(vari, vari, 1),      /*     i++; */
		INSTR_ADD_NEW(vara, varb, varsum),  /*     sum = a + b; */
		INSTR_MOVE_NEW(varb, vara),         /*     a = b; */
		INSTR_MOVE_NEW(varsum, varb),       /*     b = sum; */
		INSTR_JMP_NEW(-6),                  /* } */
		INSTR_CSLV_NEW(prevfr, 1, varsum),  /* Store return value (sum) */
		INSTR_CALL_NEW(prevfr)              /* return sum; */
	};
	
	Frame_appendInstructions(fib_frame, instrs, sizeof(instrs) / sizeof(Instruction));
	
	Frame_compileFrame(fib_frame);
	
	/* Functions for calling the fibonacci number generator */
	Frame *caller_frame = malloc(sizeof(Frame));
	Frame_init(caller_frame);
	
	uint32_t prevfr2  = Frame_allocVariable(caller_frame); /* not used right now, dummy */
	uint32_t varret   = Frame_allocVariable(caller_frame);
	uint32_t varcount = Frame_allocVariable(caller_frame);
	uint32_t varmax   = Frame_allocVariable(caller_frame);
	uint32_t varfib   = Frame_allocVariable(caller_frame);
	
	uint32_t funcfib  = Frame_allocFunction(caller_frame, fib_frame);
	
	/* Iterate over the fibonacci function 100 times, while asking for the count:th number */
	Frame_setVariable(caller_frame, varmax, (CFrame_Register) {.asU = 92});
	
	/* TODO: This code will leak memory, as the CFrames does not know when they are unused
	   as we no longer have a call-stack which we can unwind, NEEDS TO BE FIXED SOMEHOW!!! */
	Instruction caller[] = {
		INSTR_CCFF_NEW(funcfib, varfib),         /* fib = fibonacci; */
		INSTR_CALL_NEW(varfib),                  /* ret = fib(); */
		INSTR_CFFR_NEW(varfib),
		INSTR_PRINT_NEW(varret),                 /* print ret; */
		INSTR_SUB_NEW(varmax, varcount, varret), /* ret = max - count; */
		INSTR_BLTZ_NEW(varret, 8),               /* if( ! (ret < 0)) { */
		INSTR_CCFF_NEW(funcfib, varfib),         /*     fib = fibonacci; */
		INSTR_CSLV_NEW(varfib, varn, varcount),
		INSTR_CALL_NEW(varfib),                  /*     ret = fib(n = count); */
		INSTR_CFFR_NEW(varfib),
		INSTR_PRINT_NEW(varret),                 /*     print ret; */
		INSTR_ADDI_NEW(varcount, varcount, 1),   /*     count = count + 1; */
		INSTR_JMP_NEW(-8),                       /* } */
		INSTR_PRINT_NEW(varfib),                 /* print &fib; */
	};
	
	Frame_appendInstructions(caller_frame, caller, sizeof(caller) / sizeof(Instruction));
	
	Frame_compileFrame(caller_frame);
	
	Eval_execFrame(caller_frame);
	
	Frame_dtor(caller_frame);
	free(caller_frame);
	Frame_dtor(fib_frame);
	free(fib_frame);
	
#ifdef LEAKCHECK_H
	LeakCheck_printMemReport(1);
#endif
	
	return 0;
}
