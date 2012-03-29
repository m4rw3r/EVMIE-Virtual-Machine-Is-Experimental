#ifndef CLOSURE_H
#define CLOSURE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "Value.h"
#include "Instruction.h"

#ifndef FRAME_DEFAULT_NUMVARS
#  define FRAME_DEFAULT_NUMVARS 16
#endif

struct Frame;

typedef struct Frame
{
	uint32_t num_vars;
	uint32_t max_vars;
	uint32_t num_instructions;
	
	const Instruction *instructions;
	Value *variables;
	struct Frame *last_frame;
} Frame;

static inline int Frame_init(Frame *const frame)
{
	frame->variables        = malloc(sizeof(Value) * FRAME_DEFAULT_NUMVARS);
	frame->num_vars         = 0;
	frame->max_vars         = FRAME_DEFAULT_NUMVARS;
	frame->num_instructions = 0;
	frame->instructions     = NULL;
	
	return 1;
}

static inline void Frame_copy(Frame *const to, const Frame *const from)
{
	memcpy(to, from, sizeof(Frame));
	
	/* Only need to copy variables, as instructions are constant */
	to->variables = malloc(sizeof(Value) * from->max_vars);
	memcpy(to->variables, from->variables, sizeof(Value) * from->max_vars);
	
	/* TODO: Needed? Will most surely be overwritten just after the copy operation */
	to->last_frame = NULL;
}

/**
 * Makes sure the size of the variables array can contain the variables.
 */
static inline void Frame_allocVariablesArray(Frame *const frame, uint32_t num_vars)
{
	if(frame->max_vars < num_vars)
	{
		frame->max_vars *= 2;
		
		void *tmp = realloc(frame->variables, (sizeof(Value) * frame->max_vars));
		
		if( ! tmp)
		{
			/* TODO: Move error code to separate file and remove include of stdio.h */
			fprintf(stderr, "ERROR: Couldn't realloc() Context variables.");
			exit(-1);
		}
		
		frame->variables = tmp;
	}
}

/**
 * Allocates a new variable and returns its index in the Frame->variables array.
 */
static inline uint32_t Frame_allocVariable(Frame *const frame)
{
	Frame_allocVariablesArray(frame, ++frame->num_vars);
	
	return frame->num_vars - 1;
}

/**
 * Frees struct members, does not free the frame itself.
 */
static inline int Frame_dtor(Frame *const frame)
{
	/* TODO: Iterate variables and do GC on pointers */
	/* TODO: Reuse frames? */
	
	free(frame->variables);
	
	return 1;
}

static inline Value Frame_getVariable(const Frame *const frame, const uint32_t variable)
{
	assert(variable < frame->num_vars);
	
	return frame->variables[variable];
}

static inline void Frame_setVariable(const Frame *const frame, const uint32_t variable, const Value value)
{
	assert(variable < frame->num_vars);
	
	frame->variables[variable] = value;
}

int Frame_packFrame(Frame *const frame);

#endif