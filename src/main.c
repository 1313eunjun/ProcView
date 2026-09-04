#include <stdio.h>

#include "memory.h"

int main(void)
{
	MemoryInfo memory = get_memory_info();

	double total_gb = memory.total_kb / 1024.0 / 1024.0;
	double used_gb = memory.used_kb / 1024.0 / 1024.0;

	printf("ProcView - Linux System Monitor\n\n");

	printf("Memory Usage: %.2f GB / %.2f GB\n",
		used_gb,
		total_gb);

	return 0;
}
