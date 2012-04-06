#ifndef FRAME_H
#define FRAME_H 1

#include <stdlib.h>
#include <inttypes.h>

#include "Instruction.h"
#include "CArrayMacros.h"
#include "CFrame.h"

#ifndef FRAME_DEFAULT_NUMVARS
#  define FRAME_DEFAULT_NUMVARS 16
#endif
#ifndef FRAME_DEFAULT_FUNCSIZE
#  define  FRAME_DEFAULT_FUNCSIZE 8
#endif
#ifndef FRAME_DEFAULT_INSTRSIZE
#  define FRAME_DEFAULT_INSTRSIZE 16
#endif

struct Frame;

typedef struct Frame
{
	ARRAY_CDECL(variables, CFrame_Register)
	ARRAY_CDECL(functions, struct Frame)
	ARRAY_CDECL(instructions, Instruction)
	
	CFrame *compiled_frame;
} Frame;

int Frame_compileFrame(Frame *const frame);

static inline int Frame_init(Frame *const frame)
{
	ARRAY_INIT(frame->variables, CFrame_Register, FRAME_DEFAULT_NUMVARS);
	ARRAY_INIT(frame->functions, Frame, FRAME_DEFAULT_FUNCSIZE);
	ARRAY_INIT(frame->instructions, Instruction, FRAME_DEFAULT_INSTRSIZE);
	
	frame->compiled_frame = NULL;
	
	return 1;
}

/**
 * Allocates a new variable and returns its index in the Frame->variables array.
 */
static inline uint16_t Frame_allocVariable(Frame *const frame)
{
	++ARRAY_SIZE(frame->variables);
	ARRAY_MAKESIZE(frame->variables, CFrame_Register);
	
	return ARRAY_SIZE(frame->variables) - 1;
}

static inline uint16_t Frame_allocFunction(Frame *const frame, Frame *const function)
{
	++ARRAY_SIZE(frame->functions);
	ARRAY_MAKESIZE(frame->functions, Frame);
	
	frame->functions[ARRAY_SIZE(frame->functions) - 1] = *function;
	
	return ARRAY_SIZE(frame->functions) - 1;
}

/**
 * Frees struct members, does not free the frame itself.
 */
static inline int Frame_dtor(Frame *const frame)
{
	/* TODO: Iterate variables and do GC on pointers (or is this only in CFrames) ? */
	
	if(frame->compiled_frame)
	{
		free((Instruction *)frame->compiled_frame->cur_instr);
		free((CFrame *)frame->compiled_frame->functions);
		
		CFrame_free(frame->compiled_frame);
	}
	
	free(frame->variables);
	free(frame->functions);
	free(frame->instructions);
	
	return 1;
}

static inline CFrame_Register Frame_getVariable(const Frame *const frame, const uint16_t variable)
{
	assert(variable < ARRAY_MAX(frame->variables));
	
	return frame->variables[variable];
}

static inline void Frame_setVariable(const Frame *const frame, const uint16_t variable, const CFrame_Register value)
{
	assert(variable < ARRAY_MAX(frame->variables));
	
	frame->variables[variable] = value;
}

static inline void Frame_appendInstruction(Frame *const frame, const Instruction instr)
{
	++ARRAY_SIZE(frame->instructions);
	ARRAY_MAKESIZE(frame->instructions, Instruction);
	
	frame->instructions[ARRAY_SIZE(frame->instructions) - 1] = instr;
}

static inline void Frame_appendInstructions(Frame *const frame, const Instruction *instr_array, uint16_t size)
{
	uint16_t oldsize = ARRAY_SIZE(frame->instructions);
	
	ARRAY_SIZE(frame->instructions) += size;
	ARRAY_MAKESIZE(frame->instructions, Instruction);
	
	memcpy(frame->instructions + oldsize, instr_array, sizeof(Instruction) * size);
}

static inline CFrame *Frame_getCompiledFrame(Frame *const frame)
{
	Frame_compileFrame(frame);
	
	return frame->compiled_frame;
}

#endif