#include <stdio.h>
#include <unistd.h>

#include "memory.h"
#include "cpu.h"

int main(void)
{
    CpuTimes first_cpu = get_cpu_times();

    sleep(1);

    CpuTimes second_cpu = get_cpu_times();

    double cpu_usage =
        calculate_cpu_usage(first_cpu, second_cpu);

    MemoryInfo memory = get_memory_info();

    double total_gb =
        memory.total_kb / 1024.0 / 1024.0;

    double used_gb =
        memory.used_kb / 1024.0 / 1024.0;

    printf("ProcView - Linux System Monitor\n\n");

    printf("CPU Usage: %.1f%%\n", cpu_usage);

    printf("Memory Usage: %.2f GB / %.2f GB\n",
           used_gb,
           total_gb);

    return 0;
}
