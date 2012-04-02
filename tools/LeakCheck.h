#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <stdlib.h>

/*
 * Macros wrapping (m|c|re)alloc and free, sending the filename and line number
 * to LeakCheck so it can report where the memory was allocated.
 * It also wraps exit(status) to call LeakCheck_printMemReport(1)
 */
#define malloc(size)       LeakCheck_malloc(size, __FILE__, __LINE__)
#define calloc(num, size)  LeakCheck_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) LeakCheck_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr)          LeakCheck_free(ptr, __FILE__, __LINE__)
#define exit(status)       LeakCheck_exit(status)

/**
 * Prints a memory report (of unfreed memory addresses and their size and
 * the location where they were allocated) on stderr.
 * 
 * If free_leak_info is set to 1 (true), then it will free the information
 * about still allocated memory. This might cause notices from LeakCheck's
 * free() replacement if the program continues past this point, as it will
 * print an error on stderr if it frees a block it did not allocate .
 * 
 * Recommended usage:
 * <code>
 * #ifdef LEAKCHECK_H
 * LeakCheck_printMemReport(0);
 * #endif
 * </code>
 * 
 * Of course the 0 should be 1 if placed at the end of the program.
 * 
 * exit(int status) is automatically prepended with a call to 
 * LeakCheck_printMemReport(1).
 */
void LeakCheck_printMemReport(int free_leak_info);

/**
 * Wrapper for malloc() from stdlib.h
 */
void *LeakCheck_malloc(size_t size, const char *file, unsigned int line);
/**
 * Wrapper for calloc() from stdlib.h
 */
void *LeakCheck_calloc(size_t size, size_t num, const char *file, unsigned int line);
/**
 * Wrapper for realloc() from stdlib.h
 */
void *LeakCheck_realloc(void *ptr, size_t size, const char *file, unsigned int line);
/**
 * Wrapper for free() from stdlib.h
 */
void LeakCheck_free(void *ptr, const char *file, unsigned int line);
/**
 * Wrapper for exit(), calling LeakCheck_printMemReport(1).
 */
void LeakCheck_exit(int status);

#endif