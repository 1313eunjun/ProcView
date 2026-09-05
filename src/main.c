#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "memory.h"
#include "cpu.h"
#include "process.h"

#define MAX_PROCESSES 256

int main(int argc, char *argv[])
{
    int sort_by_memory = 1;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: ./procview [options]\n\n");
        printf("Options:\n");
        printf("  --sort memory   Sort processes by memory usage\n");
        printf("  --help          Show this help message\n");

        return 0;
    }

    if (argc == 3 &&
        strcmp(argv[1], "--sort") == 0 &&
        strcmp(argv[2], "memory") == 0) {

        sort_by_memory = 1;

    } else if (argc != 1) {

        printf("Invalid option.\n");
        printf("Use ./procview --help for usage information.\n");

        return 1;
    }

    /*
     * First CPU measurement
     */
    CpuTimes first_cpu = get_cpu_times();

    /*
     * First process measurement
     */
    ProcessInfo first_processes[MAX_PROCESSES];

    int first_process_count =
        get_process_list(first_processes, MAX_PROCESSES);

    /*
     * Wait one second so CPU usage can be measured
     * over a time interval.
     */
    sleep(1);

    /*
     * Second CPU measurement
     */
    CpuTimes second_cpu = get_cpu_times();

    /*
     * Second process measurement
     */
    ProcessInfo processes[MAX_PROCESSES];

    int process_count =
        get_process_list(processes, MAX_PROCESSES);

    /*
     * Overall CPU usage
     */
    double cpu_usage =
        calculate_cpu_usage(first_cpu, second_cpu);

    /*
     * Calculate how much total CPU time increased
     * during the one-second interval.
     */
    unsigned long long first_total_cpu =
        get_total_cpu_time(first_cpu);

    unsigned long long second_total_cpu =
        get_total_cpu_time(second_cpu);

    unsigned long long total_cpu_diff =
        second_total_cpu - first_total_cpu;

    /*
     * Calculate CPU usage for each process.
     */
    calculate_process_cpu_usage(
        first_processes,
        first_process_count,
        processes,
        process_count,
        total_cpu_diff
    );

    /*
     * Read overall memory information.
     */
    MemoryInfo memory = get_memory_info();

    double total_gb =
        memory.total_kb / 1024.0 / 1024.0;

    double used_gb =
        memory.used_kb / 1024.0 / 1024.0;

    /*
     * For now, memory is still the default sort mode.
     */
    if (sort_by_memory) {
        sort_processes_by_memory(processes, process_count);
    }

    printf("ProcView - Linux System Monitor\n\n");

    printf("CPU Usage: %.1f%%\n", cpu_usage);

    printf("Memory Usage: %.2f GB / %.2f GB\n\n",
           used_gb,
           total_gb);

    printf("%-8s %-25s %10s\n",
           "PID",
           "PROCESS",
           "MEMORY");

    int display_count =
        process_count < 10 ? process_count : 10;

    for (int i = 0; i < display_count; i++) {

        double memory_mb =
            processes[i].memory_kb / 1024.0;

        printf("%-8d %-25s %9.1f MB\n",
               processes[i].pid,
               processes[i].name,
               memory_mb);
    }

    return 0;
}
