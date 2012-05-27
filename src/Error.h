#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/* TODO: Proper error functions which unwind the stack? */

#define Error_fatalError(...) \
fprintf(stderr, "ERROR (%s:%d): ", __FILE__, __LINE__); \
fprintf(stderr, __VA_ARGS__);exit(-1);

#endif