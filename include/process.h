#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    int pid;
    char name[256];
} ProcessInfo;

int get_process_list(ProcessInfo processes[], int max_processes);

#endif
