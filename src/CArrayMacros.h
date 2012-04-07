#ifndef CARRAYMACROS_H
#define CARRAYMACROS_H 1

#include <inttypes.h>
#include "Error.h"


#define ARRAY_SIZE(array) array ## _num
#define ARRAY_MAX(array) array ## _max

/* TODO: Is it needed with 32-bit arrays here? I think most CFrames/Frames only need 2^16 elements */
#define ARRAY_CDECL(array, type) \
	uint16_t ARRAY_MAX(array);   \
	uint16_t ARRAY_SIZE(array);  \
	type *array;

#define ARRAY_INIT(array, type, initial_size)         \
	do { array = malloc(sizeof(type) * initial_size); \
		ARRAY_MAX(array)  = initial_size;             \
		ARRAY_SIZE(array) = 0; } while(0)

/* Resizes array of type type to at least ARRAY_SIZE(array) */ 
/* TODO: Make sure we only use powers of 2? */
#define ARRAY_MAKESIZE(array, type)                                         \
	if(ARRAY_MAX(array) < ARRAY_SIZE(array)) {                              \
	while(ARRAY_MAX(array) < ARRAY_SIZE(array)) { ARRAY_MAX(array) *= 2; }  \
	void *tmp = realloc(array, (sizeof(type) * ARRAY_MAX(array)));          \
	if( ! tmp) {                                                            \
		Error_fatalError("ERROR: Couldn't realloc() Frame variables.");     \
	} array = tmp; }

/* Allocates smallest possible chunk of memory for the source array and puts it in dest */
#define ARRAY_PACK(array, type) \
	do { type *tmp = realloc(array, (sizeof(type) * ARRAY_SIZE(array))); \
	if( ! tmp) {                                                         \
		Error_fatalError("ERROR: Couldn't realloc() array.");            \
	} array = tmp; } while(0)

#endif