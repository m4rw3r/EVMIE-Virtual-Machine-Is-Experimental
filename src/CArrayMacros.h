#ifndef CARRAYMACROS_H
#define CARRAYMACROS_H 1

#include <inttypes.h>
#include "Error.h"

#define ARRAY_SIZE(array) array ## _num
#define ARRAY_MAX(array) array ## _max

#define ARRAY_CDECL(array, type, bitsize) \
	uint ## bitsize ## _t ARRAY_MAX(array);   \
	uint ## bitsize ## _t ARRAY_SIZE(array);  \
	type *array;

#define ARRAY8_CDECL(array, type) ARRAY_CDECL(array, type, 8)
#define ARRAY16_CDECL(array, type) ARRAY_CDECL(array, type, 16)
#define ARRAY32_CDECL(array, type) ARRAY_CDECL(array, type, 32)
#define ARRAY64_CDECL(array, type) ARRAY_CDECL(array, type, 64)

#define ARRAY_INIT(array, type, initial_size)         \
	do { array = malloc(sizeof(type) * initial_size); \
		ARRAY_MAX(array)  = initial_size;             \
		ARRAY_SIZE(array) = 0; } while(0)

/* Resizes array of type type to at least ARRAY_SIZE(array) */
/* TODO: Make sure we only use powers of 2? */
#define ARRAY_MAKESIZE(array, type, bitsize)                                \
	if(ARRAY_MAX(array) < ARRAY_SIZE(array)) {                              \
	while(ARRAY_MAX(array) < ARRAY_SIZE(array)) {                           \
		if(UINT ## bitsize ## _MAX / ARRAY_MAX(array) < 2) {                \
			Error_fatalError("ARRAY_MAKESIZE: Couldn't realloc() array, size overflow."); \
		} ARRAY_MAX(array) *= 2; }                                          \
	void *tmp = realloc(array, (sizeof(type) * ARRAY_MAX(array)));          \
	if( ! tmp) {                                                            \
		Error_fatalError("ARRAY_MAKESIZE: Couldn't realloc() array.\n");          \
	} array = tmp; }

#define ARRAY8_MAKESIZE(array, type) ARRAY_MAKESIZE(array, type, 8)
#define ARRAY16_MAKESIZE(array, type) ARRAY_MAKESIZE(array, type, 16)
#define ARRAY32_MAKESIZE(array, type) ARRAY_MAKESIZE(array, type, 32)
#define ARRAY64_MAKESIZE(array, type) ARRAY_MAKESIZE(array, type, 64)

/* Allocates smallest possible chunk of memory for the source array and puts it in dest */
#define ARRAY_PACK(array, type) \
	do { type *tmp = realloc(array, (sizeof(type) * ARRAY_SIZE(array))); \
	if( ! tmp) {                                                         \
		Error_fatalError("ARRAY_PACK: Couldn't realloc() array.\n");     \
	} array = tmp; } while(0)

#endif