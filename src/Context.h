#ifndef CONTEXT_H
#define CONTEXT_H 1

/* TODO: Move stderr part from Context_allocVariablesArray into some shared error object */
#include <stdio.h>
#include <stdlib.h>
#include "Value.h"

#define CONTEXT_DEFAULT_HEAPSIZE 16

typedef struct Context {
	Value *variables;
	uint64_t num_vars;
	uint64_t max_vars; /* Allocated size */
} Context;

static inline int Context_init(Context *const ctx)
{
	ctx->variables = malloc(sizeof(Value) * CONTEXT_DEFAULT_HEAPSIZE);
	ctx->num_vars = 0;
	ctx->max_vars = CONTEXT_DEFAULT_HEAPSIZE;
	
	return 1;
}

static inline int Context_dtor(Context *const ctx)
{
	free(ctx->variables);
	
	return 1;
}

/**
 * Makes sure the size of the variables array can contain the variables.
 */
static inline void Context_allocVariablesArray(Context *const ctx, uint64_t num_vars)
{
	if(ctx->max_vars < num_vars)
	{
		ctx->max_vars *= 2;
		
		void *tmp = realloc(ctx->variables, (sizeof(Value) * ctx->max_vars));
		
		if( ! tmp)
		{
			fprintf(stderr, "ERROR: Couldn't realloc() Context variables.");
			exit(-1);
		}
		
		ctx->variables = tmp;
	}
}

/**
 * Allocates a new variable and returns its address in the context array.
 */
static inline uint64_t Context_allocVariable(Context *const ctx)
{
	ctx->num_vars += 1;
	
	Context_allocVariablesArray(ctx, ctx->num_vars);
	
	return ctx->num_vars - 1;
}

static inline Value Context_getVariable(Context *const ctx, const uint64_t variable)
{
	assert(variable < ctx->num_vars);
	
	return ctx->variables[variable];
}

static inline void Context_setVariable(Context *const ctx, const uint64_t variable, const Value value)
{
	assert(variable < ctx->num_vars);
	
	ctx->variables[variable] = value;
}

#endif