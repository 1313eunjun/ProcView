#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "process.h"


static unsigned long long get_process_cpu_time(int pid)
{
    char path[256];

    snprintf(path,
             sizeof(path),
             "/proc/%d/stat",
             pid);

    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return 0;
    }

    char buffer[1024];

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return 0;
    }

    fclose(file);

    /*
     * /proc/<PID>/stat starts like:
     *
     * 3093 (bash) S ...
     *
     * The process name is inside parentheses and may contain spaces,
     * so find the final ')' first.
     */
    char *right_parenthesis = strrchr(buffer, ')');

    if (right_parenthesis == NULL) {
        return 0;
    }

    /*
     * Move past ") " so data now starts at field 3 (state).
     */
    char *data = right_parenthesis + 2;

    unsigned long long utime = 0;
    unsigned long long stime = 0;

    int result = sscanf(
        data,
        "%*c "
        "%*d %*d %*d %*d %*d "
        "%*u %*u %*u %*u %*u "
        "%llu %llu",
        &utime,
        &stime
    );

    if (result != 2) {
        return 0;
    }

    return utime + stime;
}


int get_process_list(ProcessInfo processes[], int max_processes)
{
    DIR *proc_dir = opendir("/proc");

    if (proc_dir == NULL) {
        perror("Failed to open /proc");
        return 0;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(proc_dir)) != NULL) {

        /*
         * Process directories in /proc have numeric names:
         *
         * /proc/1
         * /proc/3093
         * /proc/4500
         *
         * Ignore entries such as meminfo, cpuinfo, stat, etc.
         */
        if (!isdigit(entry->d_name[0])) {
            continue;
        }

        if (count >= max_processes) {
            break;
        }

        int pid = atoi(entry->d_name);

        char path[256];

        /*
         * Read the process name from:
         *
         * /proc/<PID>/comm
         */
        snprintf(path,
                 sizeof(path),
                 "/proc/%d/comm",
                 pid);

        FILE *file = fopen(path, "r");

        if (file == NULL) {
            continue;
        }

        if (fgets(processes[count].name,
                  sizeof(processes[count].name),
                  file) == NULL) {

            fclose(file);
            continue;
        }

        fclose(file);

        /*
         * fgets() normally keeps the newline.
         *
         * "bash\n" -> "bash"
         */
        processes[count].name[
            strcspn(processes[count].name, "\n")
        ] = '\0';

        processes[count].pid = pid;

        /*
         * Default memory value in case VmRSS cannot be read.
         */
        processes[count].memory_kb = 0;

        /*
         * Read process memory information from:
         *
         * /proc/<PID>/status
         */
        snprintf(path,
                 sizeof(path),
                 "/proc/%d/status",
                 pid);

        file = fopen(path, "r");

        if (file != NULL) {

            char line[256];

            while (fgets(line, sizeof(line), file)) {

                if (sscanf(line,
                           "VmRSS: %ld kB",
                           &processes[count].memory_kb) == 1) {
                    break;
                }
            }

            fclose(file);
        }

        /*
         * Read cumulative process CPU time:
         *
         * cpu_time = utime + stime
         */
        processes[count].cpu_time =
            get_process_cpu_time(pid);

        /*
         * We have not calculated the CPU percentage yet.
         * That requires two measurements.
         */
        processes[count].cpu_usage = 0.0;

        count++;
    }

    closedir(proc_dir);

    return count;
}


static int compare_memory(const void *a, const void *b)
{
    const ProcessInfo *process_a = a;
    const ProcessInfo *process_b = b;

    if (process_a->memory_kb < process_b->memory_kb) {
        return 1;
    }

    if (process_a->memory_kb > process_b->memory_kb) {
        return -1;
    }

    return 0;
}


void sort_processes_by_memory(ProcessInfo processes[], int count)
{
    qsort(processes,
          count,
          sizeof(ProcessInfo),
          compare_memory);
}
void calculate_process_cpu_usage(
    ProcessInfo first[],
    int first_count,
    ProcessInfo second[],
    int second_count,
    unsigned long long total_cpu_diff)
{
    if (total_cpu_diff == 0) {
        return;
    }

    for (int i = 0; i < second_count; i++) {

        second[i].cpu_usage = 0.0;

        for (int j = 0; j < first_count; j++) {

            if (second[i].pid == first[j].pid) {

                unsigned long long process_diff =
                    second[i].cpu_time - first[j].cpu_time;

                second[i].cpu_usage =
                    100.0 * process_diff / total_cpu_diff;

                break;
            }
        }
    }
}

static int compare_cpu(const void *a, const void *b)
{
    const ProcessInfo *process_a = a;
    const ProcessInfo *process_b = b;

    if (process_a->cpu_usage < process_b->cpu_usage) {
        return 1;
    }

    if (process_a->cpu_usage > process_b->cpu_usage) {
        return -1;
    }

    return 0;
}

void sort_processes_by_cpu(ProcessInfo processes[], int count)
{
    qsort(processes,
          count,
          sizeof(ProcessInfo),
          compare_cpu);
}
