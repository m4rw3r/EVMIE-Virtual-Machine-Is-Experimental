#ifndef CFRAME_H
#define CFRAME_H 1

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "Value.h"
#include "Instruction.h"

struct CFrame;

typedef struct CFrame
{
	uint32_t num_vars;
	/* List of CFrames which can/will be called from local scope */
	const struct CFrame * functions;
	/* Current instruction pointer, points to an array of instructions allocated
	   by Frame_compileFrame() */
	const Instruction *cur_instr;
	/* Last instruction pointer */
	const Instruction *last_instr;
	Value *variables;
	struct CFrame *prev_frame;
} CFrame;

static inline CFrame *CFrame_alloc()
{
	/* TODO: Reuse CFrames, needs some kind of buffer for type of CFrame */
	printf("Allocating new CFrame\n");
	return malloc(sizeof(CFrame));
}

static inline void CFrame_copy(CFrame *const to, const CFrame *const from)
{
	memcpy(to, from, sizeof(CFrame));
	
	/* Only need to copy variables, as instructions are constant */
	to->variables = malloc(sizeof(Value) * from->num_vars);
	memcpy(to->variables, from->variables, sizeof(Value) * from->num_vars);
	
	/* TODO: Needed? Will most surely be overwritten just after the copy operation */
	to->prev_frame = NULL;
}

/**
 * Frees struct members, includes struct.
 * 
 * Does not free instruction array or function array.
 */
static inline int CFrame_free(CFrame *const frame)
{
	/* TODO: Iterate variables and do GC on pointers */
	/* TODO: Reuse CFrames */
	
	free(frame->variables);
	
	/* No need to free instructions or functions, they are freed upon Frame_dtor()
	   frees the compiled_frame */
	free(frame);
	
	printf("Freeing CFrame\n");
	
	return 1;
}

static inline Value CFrame_getVariable(const CFrame *const frame, const uint32_t variable)
{
	assert(variable < frame->num_vars);
	
	return frame->variables[variable];
}

static inline void CFrame_setVariable(const CFrame *const frame, const uint32_t variable, const Value value)
{
	assert(variable < frame->num_vars);
	
	frame->variables[variable] = value;
}

#endif