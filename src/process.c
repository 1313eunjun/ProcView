#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "process.h"

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

        if (!isdigit(entry->d_name[0])) {
            continue;
        }

        if (count >= max_processes) {
            break;
        }

        int pid = atoi(entry->d_name);

        char path[256];

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

        processes[count].name[
            strcspn(processes[count].name, "\n")
        ] = '\0';

        processes[count].pid = pid;
        processes[count].memory_kb = 0;

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
