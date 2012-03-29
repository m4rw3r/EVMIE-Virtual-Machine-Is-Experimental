#ifndef CARRAYMACROS_H
#define CARRAYMACROS_H 1

#include <inttypes.h>
#include <stdio.h>


#define ARRAY_SIZE(array) array ## _num
#define ARRAY_MAX(array) array ## _max

#define ARRAY_CDECL(array, type) \
	uint32_t ARRAY_MAX(array);   \
	uint32_t ARRAY_SIZE(array);  \
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
		/* TODO: Move error code to separate file and remove                \
		         include of stdio.h */                                      \
		fprintf(stderr, "ERROR: Couldn't realloc() Frame variables.");      \
		exit(-1);                                                           \
	} array = tmp; }

/* Allocates smallest possible chunk of memory for the source array and puts it in dest */
#define ARRAY_PACK(array, type) \
	do { type *tmp = realloc(array, (sizeof(type) * ARRAY_SIZE(array))); \
	if( ! tmp) {                                                         \
		/* TODO: Move error code to separate file and remove             \
		         include of stdio.h */                                   \
		fprintf(stderr, "ERROR: Couldn't realloc() array.");             \
		exit(-1);                                                        \
	} array = tmp; } while(0)

#endif