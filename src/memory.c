#include <stdio.h>
#include <string.h>

#include "memory.h"

MemoryInfo get_memory_info(void)
{
	FILE *file = fopen("/proc/meminfo", "r");

	MemoryInfo info = {0};

	if(file == NULL) {
		perror("Failed to open /proc/meminfo");
		return info;
	}

	char line[256];

	while (fgets(line, sizeof(line), file)) {

		if(sscanf(line, "MemTotal: %ld kB", &info.total_kb) == 1) {
			continue;
		}

		if(sscanf(line, "MemAvailable: %ld kB", &info.available_kb) == 1) {
			continue;
		}
	}

	fclose(file);

	info.used_kb = info.total_kb - info.available_kb;

	return info;
}
