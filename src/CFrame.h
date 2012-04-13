#ifndef CFRAME_H
#define CFRAME_H 1

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include "StaticAssert.h"
#include "config.h"
#include "Instruction.h"

typedef union CFrame_Register {
	uint64_t asU;
	int64_t  asI;
	double   asF;
} CFrame_Register;
#include "Object.h"

struct CFrame;

typedef struct CFrame
{
	Object_ObjectHeader
	/* List of CFrames which can/will be called from local scope */
	const struct CFrame *functions;
	/* Pointers to instructions */
	const Instruction *first_instr;
	const Instruction *last_instr;
	/* Current instruction pointer, points to an array of instructions allocated
	   by Frame_compileFrame() */
	const Instruction *cur_instr;
#if VM_PROFILE
	/* Number of cycles spent in this CFrame */
	uint64_t cycles;
#endif
} CFrame;

StaticAssert(sizeof(CFrame_Register) == 8, "CFrame_Register struct is not 64 bit large.");

static inline CFrame *CFrame_alloc()
{
	/* TODO: Reuse CFrames, needs some kind of buffer for type of CFrame */
	CFrame *tmp = malloc(sizeof(CFrame));
	memset(tmp, 0, sizeof(CFrame));
	printf("Allocating new CFrame %p\n", (void *)tmp);
	return tmp;
}

static inline void CFrame_copy(CFrame *const to, const CFrame *const from)
{
	memcpy(to, from, sizeof(CFrame));
	
	/* Only need to copy variables, as instructions are constant */
	to->variables = malloc(sizeof(CFrame_Register) * from->num_vars);
	memcpy(to->variables, from->variables, sizeof(CFrame_Register) * from->num_vars);
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
	
	printf("Freeing CFrame %p\n", (void *) frame);
	
	free(frame->variables);
	
	/* No need to free instructions or functions, they are freed upon Frame_dtor()
	   frees the compiled_frame */
	free(frame);
	
	return 1;
}

static inline void CFrame_incRefc(CFrame *const frame)
{
	frame->refcount++;
	printf("Frame %p r: %d\n", (void *)frame, frame->refcount);
}

static inline void CFrame_decRefc(CFrame *const frame)
{
	frame->refcount--;
	
	printf("Frame %p r: %d\n", (void *)frame, frame->refcount);
	
	
	if(frame->refcount == 0) {
		if(frame->variables[0].asU != 0) {
			CFrame_decRefc((CFrame *)frame->variables[0].asU);
		}
		
		CFrame_free(frame);
	}
}

static inline CFrame_Register CFrame_getVariable(const CFrame *const frame, const uint16_t variable)
{
	assert(variable < frame->num_vars);
	
	return frame->variables[variable];
}

static inline void CFrame_setVariable(const CFrame *const frame, const uint16_t variable, const CFrame_Register value)
{
	assert(variable < frame->num_vars);
	
	frame->variables[variable] = value;
}

#endif