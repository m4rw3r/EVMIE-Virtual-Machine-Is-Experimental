
#include "Frame.h"
#include "Error.h"
#include <stdio.h>

/**
 * Call to compress the frame (allocate new Instruction array and realloc variables into
 * smallest possible size) before usage in Eval.
 */
int Frame_compileFrame(Frame *const frame)
{
	if(frame->compiled_frame)
	{
		return 0;
	}
	
	if(ARRAY_SIZE(frame->instructions) == 0) {
		/* TODO: Maybe just return false? or some other status code?
		         might be best to introduce an enum of return codes */
		Error_fatalError("Attempted to compile frame without instructions\n");
	}
	
	CFrame *cframe = CFrame_alloc();
	
	/* Pack the instruction array into continuous block */
	ARRAY_PACK(frame->instructions, Instruction);
	cframe->first_instr = (const Instruction*) frame->instructions;
	cframe->cur_instr   = (const Instruction*) frame->instructions;
	cframe->last_instr  = &cframe->first_instr[ARRAY_SIZE(frame->instructions) - 1];
	
	uint32_t i;
	CFrame *related_frames = malloc(sizeof(CFrame) * ARRAY_SIZE(frame->functions));
	for(i = 0; i < ARRAY_SIZE(frame->functions); i++)
	{
		Frame_compileFrame(&frame->functions[i]);
		
		related_frames[i] = *frame->functions[i].compiled_frame;
	}
	
	cframe->functions = related_frames;
	/* No need to save size, as they won't be copied */
	
	/* Realloc the variables array, to make for less memory allocated for each frame
	   when running */
	ARRAY_PACK(frame->variables, Value);
	cframe->variables = frame->variables;
	cframe->variables_num = ARRAY_SIZE(frame->variables);
	
	frame->compiled_frame = cframe;
	
	return 1;
}
