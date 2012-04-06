
/* sprintf */
#include <stdio.h>
#include <stdlib.h>

#include "Instruction.h"
#include "Eval.h"
#include "Frame.h"

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
	uint64_t vartmp = Frame_allocVariable(frame);
	
	Instruction instrs[] = {
		INSTR_NOP_NEW(),
		/* varsum == 0 */
		INSTR_ADDI_NEW(varn, varn, 20),
		/* vara == 0 */
		INSTR_ADDI_NEW(varb, varb, 1),
		/* vari == 0 */
		INSTR_SUB_NEW(varn, vari, vartmp),
		INSTR_BLTZ_NEW(vartmp, 8),
		INSTR_ADDI_NEW(vari, vari, 1),
		INSTR_ADD_NEW(vara, varb, varsum),
		INSTR_MOVE_NEW(varb, vara),
		INSTR_MOVE_NEW(varsum, varb),
		INSTR_PRINT_NEW(vari),
		INSTR_PRINT_NEW(varsum),
		INSTR_JMP_NEW(-8),
		INSTR_PRINT_NEW(varsum)
	};
	
	Frame_appendInstructions(frame, instrs, sizeof(i) / sizeof(Instruction));
	
	Frame_compileFrame(frame);
	
	Eval_execFrame(frame);
	
	return 0;
}
