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
     * Default refresh interval.
     */
    int refresh_interval = 2;

    /*
     * Read command-line arguments one by one.
     */
    for (int i = 1; i < argc; i++) {

        /*
         * --help
         */
        if (strcmp(argv[i], "--help") == 0) {

            printf("Usage: ./procview [options]\n\n");

            printf("Options:\n");
            printf("  --sort memory   Sort processes by memory usage\n");
            printf("  --sort cpu      Sort processes by CPU usage\n");
            printf("  --interval N    Refresh every N seconds\n");
            printf("  --help          Show this help message\n");

            return 0;
        }

        /*
         * --sort memory
         * --sort cpu
         */
        else if (strcmp(argv[i], "--sort") == 0) {

            /*
             * Make sure a value exists after --sort.
             */
            if (i + 1 >= argc) {
                printf("Missing value after --sort.\n");
                return 1;
            }

            /*
             * Move to the value after --sort.
             */
            i++;

            if (strcmp(argv[i], "memory") == 0) {

                sort_mode = 0;

            } else if (strcmp(argv[i], "cpu") == 0) {

                sort_mode = 1;

            } else {

                printf("Invalid sort option: %s\n", argv[i]);
                return 1;
            }
        }

        /*
         * --interval N
         */
        else if (strcmp(argv[i], "--interval") == 0) {

            /*
             * Make sure a value exists after --interval.
             */
            if (i + 1 >= argc) {
                printf("Missing value after --interval.\n");
                return 1;
            }

            /*
             * Move to the interval value.
             */
            i++;

            refresh_interval = atoi(argv[i]);

            if (refresh_interval <= 0) {
                printf("Interval must be greater than 0.\n");
                return 1;
            }
        }

        /*
         * Anything else is invalid.
         */
        else {

            printf("Invalid option: %s\n", argv[i]);
            printf("Use ./procview --help for usage information.\n");

            return 1;
        }
    }

    /*
     * Keep monitoring until Ctrl + C.
     */
    while (1) {

        /*
         * First overall CPU measurement.
         */
        CpuTimes first_cpu =
            get_cpu_times();

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
         * Wait one second so CPU usage
         * can be measured.
         */
        sleep(1);

        /*
         * Second overall CPU measurement.
         */
        CpuTimes second_cpu =
            get_cpu_times();

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
         * Overall CPU usage.
         */
        double cpu_usage =
            calculate_cpu_usage(
                first_cpu,
                second_cpu
            );

        /*
         * Total CPU time difference.
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
         * Read memory information.
         */
        MemoryInfo memory =
            get_memory_info();

        double total_gb =
            memory.total_kb / 1024.0 / 1024.0;

        double used_gb =
            memory.used_kb / 1024.0 / 1024.0;

        /*
         * Sort processes.
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
         * Clear terminal screen.
         */
        printf("\033[2J\033[H");

        /*
         * System information.
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
         * Process table header.
         */
        printf("%-8s %-25s %8s %10s\n",
               "PID",
               "PROCESS",
               "CPU",
               "MEMORY");

        /*
         * Display top 10 processes.
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
         * Display output immediately.
         */
        fflush(stdout);

        /*
         * CPU measurement already takes about 1 second,
         * so wait only the remaining interval.
         */
        if (refresh_interval > 1) {
            sleep(refresh_interval - 1);
        }
    }

    return 0;
}
