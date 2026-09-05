#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memory.h"
#include "cpu.h"
#include "process.h"

#define MAX_PROCESSES 256

int main(int argc, char *argv[])
{
    /*
     * sort_mode:
     * 0 = sort by memory
     * 1 = sort by CPU
     */
    int sort_mode = 0;

    /*
     * Default refresh interval in seconds.
     */
    int refresh_interval = 2;

    /*
     * Handle --help
     */
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {

        printf("Usage: ./procview [options]\n\n");

        printf("Options:\n");
        printf("  --sort memory   Sort processes by memory usage\n");
        printf("  --sort cpu      Sort processes by CPU usage\n");
        printf("  --interval N    Refresh every N seconds\n");
        printf("  --help          Show this help message\n");

        return 0;
    }

    /*
     * Handle --sort memory
     * and --sort cpu
     */
    if (argc == 3 && strcmp(argv[1], "--sort") == 0) {

        if (strcmp(argv[2], "memory") == 0) {

            sort_mode = 0;

        } else if (strcmp(argv[2], "cpu") == 0) {

            sort_mode = 1;

        } else {

            printf("Invalid sort option: %s\n", argv[2]);
            printf("Use ./procview --help for usage information.\n");

            return 1;
        }

    /*
     * Handle --interval N
     */
    } else if (argc == 3 &&
               strcmp(argv[1], "--interval") == 0) {

        refresh_interval = atoi(argv[2]);

        if (refresh_interval <= 0) {

            printf("Interval must be greater than 0.\n");

            return 1;
        }

    } else if (argc != 1) {

        printf("Invalid option.\n");
        printf("Use ./procview --help for usage information.\n");

        return 1;
    }

    /*
     * Keep monitoring until Ctrl + C is pressed.
     */
    while (1) {

        /*
         * First overall CPU measurement.
         */
        CpuTimes first_cpu = get_cpu_times();

        /*
         * First process measurement.
         */
        ProcessInfo first_processes[MAX_PROCESSES];

        int first_process_count =
            get_process_list(
                first_processes,
                MAX_PROCESSES
            );

        /*
         * Wait one second so CPU usage can be measured.
         */
        sleep(1);

        /*
         * Second overall CPU measurement.
         */
        CpuTimes second_cpu = get_cpu_times();

        /*
         * Second process measurement.
         */
        ProcessInfo processes[MAX_PROCESSES];

        int process_count =
            get_process_list(
                processes,
                MAX_PROCESSES
            );

        /*
         * Calculate overall CPU usage.
         */
        double cpu_usage =
            calculate_cpu_usage(
                first_cpu,
                second_cpu
            );

        /*
         * Calculate total CPU time difference.
         */
        unsigned long long first_total_cpu =
            get_total_cpu_time(first_cpu);

        unsigned long long second_total_cpu =
            get_total_cpu_time(second_cpu);

        unsigned long long total_cpu_diff =
            second_total_cpu - first_total_cpu;

        /*
         * Calculate per-process CPU usage.
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
        MemoryInfo memory =
            get_memory_info();

        double total_gb =
            memory.total_kb / 1024.0 / 1024.0;

        double used_gb =
            memory.used_kb / 1024.0 / 1024.0;

        /*
         * Sort process list.
         */
        if (sort_mode == 0) {

            sort_processes_by_memory(
                processes,
                process_count
            );

        } else {

            sort_processes_by_cpu(
                processes,
                process_count
            );
        }

        /*
         * Clear terminal and move cursor
         * to the top-left.
         */
        printf("\033[2J\033[H");

        /*
         * Print system summary.
         */
        printf("ProcView - Linux System Monitor\n\n");

        printf("CPU Usage: %.1f%%\n",
               cpu_usage);

        printf("Memory Usage: %.2f GB / %.2f GB\n",
               used_gb,
               total_gb);

        printf("Refresh Interval: %d seconds\n\n",
               refresh_interval);

        /*
         * Print process table header.
         */
        printf("%-8s %-25s %8s %10s\n",
               "PID",
               "PROCESS",
               "CPU",
               "MEMORY");

        /*
         * Show only the top 10 processes.
         */
        int display_count =
            process_count < 10
            ? process_count
            : 10;

        for (int i = 0;
             i < display_count;
             i++) {

            double memory_mb =
                processes[i].memory_kb / 1024.0;

            printf("%-8d %-25s %7.1f%% %9.1f MB\n",
                   processes[i].pid,
                   processes[i].name,
                   processes[i].cpu_usage,
                   memory_mb);
        }

        /*
         * Immediately display buffered output.
         */
        fflush(stdout);

        /*
         * Wait before next refresh cycle.
         */
        if(refresh_interval > 1) {
		sleep(refresh_interval - 1);
	}
    }

    return 0;
}
