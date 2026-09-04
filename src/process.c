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

        count++;
    }

    closedir(proc_dir);

    return count;
}
