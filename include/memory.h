#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
	long total_kb;
	long available_kb;
	long used_kb;
} MemoryInfo;

MemoryInfo get_memory_info(void);

#endif

