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

typedef uint8_t var_index;
#define VAR_INDEX_MAX 255

struct Frame;

typedef struct Frame
{
	ARRAY8_CDECL(variables, Value)
	ARRAY8_CDECL(functions, struct Frame)
	ARRAY32_CDECL(instructions, Instruction)
	
	CFrame *compiled_frame;
} Frame;

/* TODO: Make Frame immutable once it has been compiled? Needs to be imho otherwise it might cause
         strange behaviour when modifying running code */

int Frame_compileFrame(Frame *const frame);

static inline int Frame_init(Frame *const frame)
{
	ARRAY_INIT(frame->variables, Value, FRAME_DEFAULT_NUMVARS);
	ARRAY_INIT(frame->functions, Frame, FRAME_DEFAULT_FUNCSIZE);
	ARRAY_INIT(frame->instructions, Instruction, FRAME_DEFAULT_INSTRSIZE);
	
	frame->compiled_frame = NULL;
	
	return 1;
}

/**
 * Allocates a new variable and returns its index in the Frame->variables array.
 */
static inline var_index Frame_allocVariable(Frame *const frame)
{
	/* TODO: Check max-size for variables (ie. prevent wraparound) */
	++ARRAY_SIZE(frame->variables);
	ARRAY8_MAKESIZE(frame->variables, Value);
	
	return ARRAY_SIZE(frame->variables) - 1;
}

static inline var_index Frame_allocFunction(Frame *const frame, Frame *const function)
{
	++ARRAY_SIZE(frame->functions);
	ARRAY16_MAKESIZE(frame->functions, Frame);
	
	frame->functions[ARRAY_SIZE(frame->functions) - 1] = *function;
	
	return ARRAY_SIZE(frame->functions) - 1;
}

/**
 * Frees struct members, does not free the frame itself.
 */
static inline int Frame_dtor(Frame *const frame)
{
	/* TODO: Iterate variables and do GC on pointers (or is this only in CFrames) ? */
	
	if(frame->compiled_frame) {
		free((CFrame *)frame->compiled_frame->functions);
		
		CFrame_free(frame->compiled_frame);
	} else {
		/* Freed in CFrame_free() */
		free(frame->variables); 
	}
	
	free(frame->functions);
	free(frame->instructions);
	
	return 1;
}

static inline Value Frame_getVariable(const Frame *const frame, const var_index variable)
{
	assert(variable < ARRAY_MAX(frame->variables));
	
	return frame->variables[variable];
}

static inline void Frame_setVariable(const Frame *const frame, const var_index variable, const Value value)
{
	assert(variable < ARRAY_MAX(frame->variables));
	
	frame->variables[variable] = value;
}

static inline void Frame_appendInstruction(Frame *const frame, const Instruction instr)
{
	++ARRAY_SIZE(frame->instructions);
	ARRAY32_MAKESIZE(frame->instructions, Instruction);
	
	frame->instructions[ARRAY_SIZE(frame->instructions) - 1] = instr;
}

static inline void Frame_appendInstructions(Frame *const frame, const Instruction *instr_array, uint16_t size)
{
	uint16_t oldsize = ARRAY_SIZE(frame->instructions);
	
	ARRAY_SIZE(frame->instructions) += size;
	ARRAY32_MAKESIZE(frame->instructions, Instruction);
	
	memcpy(frame->instructions + oldsize, instr_array, sizeof(Instruction) * size);
}

static inline CFrame *Frame_getCompiledFrame(Frame *const frame)
{
	Frame_compileFrame(frame);
	
	return frame->compiled_frame;
}

#endif