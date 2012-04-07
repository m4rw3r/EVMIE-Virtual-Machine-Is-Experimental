#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define Error_fatalError(...) \
fprintf(stderr, __VA_ARGS__);exit(-1);

#endif