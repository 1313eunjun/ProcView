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

        processes[count].pid = atoi(entry->d_name);

        count++;
    }

    closedir(proc_dir);

    return count;
}
