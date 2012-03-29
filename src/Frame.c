
#include "Frame.h"
#include <stdio.h>

/**
 * Call to compress the frame (allocate new Instruction array and realloc variables into
 * smallest possible size) before usage in Eval.
 */
int Frame_packFrame(Frame *const frame)
{
	/* TODO: Is this really needed here for instructions? Isn't there a better way to
	         do it elsewhere (like where the Instruction array is assembled) ? */
	uint32_t num_instructions = 0; 
	const Instruction *instr = frame->instructions;
	
	for(; instr != NULL; instr = instr->next, num_instructions++)
	{
		/* Empty */
	}
	
	/* TODO: Where to free() this? */
	Instruction *instrarray = malloc(sizeof(Instruction) * num_instructions);
	instr = frame->instructions;
	int i = 0;
	
	for(; instr != NULL; instr = instr->next, i++)
	{
		instrarray[i] = *instr;
	}
	
	frame->instructions = instrarray;
	frame->num_instructions = num_instructions;
	
	/* Realloc the variables array, to make for less memory allocated for each frame
	   when running */
	void *tmp = realloc(frame->variables, (sizeof(Value) * frame->num_vars));
	
	if( ! tmp)
	{
		/* TODO: Move error code to separate file and remove include of stdio.h */
		fprintf(stderr, "ERROR: Couldn't realloc() Context variables.");
		exit(-1);
	}
	
	frame->variables = tmp;
	frame->max_vars  = frame->num_vars;
	
	return 1;
}
