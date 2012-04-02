
#include <stdlib.h>
#include <stdio.h>

struct MemInfo;
typedef struct MemInfo {
	void *ptr;
	size_t size;
	const char *file;
	unsigned int line;
	struct MemInfo *next;
} MemInfo;

static MemInfo *memInfoLast  = NULL;

static void LeakCheck_addMemPtr(void *ptr, size_t size, const char *file, const unsigned int line)
{
	MemInfo *info = malloc(sizeof(MemInfo));
	
	info->ptr  = ptr;
	info->size = size;
	info->file = file;
	info->line = line;
	info->next = NULL;
	
	if(memInfoLast == NULL) {
		memInfoLast  = info;
	} else {
		info->next = memInfoLast;
		memInfoLast = info;
	}
}

static void LeakCheck_removeMemPtr(void *ptr, const char *file, const unsigned int line)
{
	MemInfo *cur;
	MemInfo *prev;
	
	for(cur = memInfoLast; cur; cur = cur->next) {
		if(cur->ptr == ptr) {
			if(cur == memInfoLast) {
				memInfoLast = cur->next;
			} else {
				for(prev = memInfoLast; prev; prev = prev->next) {
					if(prev->next == cur) {
						prev->next = cur->next;
						
						break;
					}
				}
			}
			
			cur->next = NULL;
			free(cur);
			return;
		}
	}
	
	/* Failure, ptr is not allocated, just alert the user that it might be a sign of a problem */
	fprintf(stderr, "LeakCheck: Attempted to free unallocated pointer %p on %s:%d\n",
		ptr, file, line);
}

void LeakCheck_printMemReport(int free_leak_info)
{
	MemInfo *info;
	
	fprintf(stderr, "\n\nMEMORY LEAK CHECK\n"
	                    "=================\n\n");
	
	size_t total_leaked = 0;
	size_t total_size = 0;
	for(info = memInfoLast; info; info = info->next) {
		fprintf(stderr, "Leaked %4zu bytes at %p from %s:%d\n",
			info->size, info->ptr, info->file, info->line);
		total_size += info->size;
		total_leaked++;
	}
	
	fprintf(stderr, "\nTotal leaked: %zu bytes in %zu chunks\n\n",
		total_size, total_leaked);
	
	if(free_leak_info) {
		MemInfo *next;
		
		for(info = memInfoLast; info; info = next) {
			next = info->next;
			free(info);
		}
		
		/* Reset so we can continue to record mallocs/frees */
		memInfoLast = NULL;
	}
}

void *LeakCheck_malloc(size_t size, const char *file, const unsigned int line)
{
	void *ptr = malloc(size);
	
	if(ptr != NULL) {
		LeakCheck_addMemPtr(ptr, size, file, line);
	}
	
	return ptr;
}
void *LeakCheck_calloc(size_t num, size_t size, const char *file, unsigned int line)
{
	size_t total = size * num;
	void *ptr = calloc(size, num);
	
	if(ptr != NULL) {
		LeakCheck_addMemPtr(ptr, total, file, line);
	}
	
	return ptr;
}
void *LeakCheck_realloc(void *ptr, size_t size, const char *file, unsigned int line)
{
	void *newptr = realloc(ptr, size);
	
	if(newptr != NULL) {
		LeakCheck_removeMemPtr(ptr, file, line);
		LeakCheck_addMemPtr(ptr, size, file, line);
	}
	
	return newptr;
}
void LeakCheck_free(void *ptr, const char *file, unsigned int line)
{
	if(ptr != NULL) {
		LeakCheck_removeMemPtr(ptr, file, line);
	}
	
	free(ptr);
}
