#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    int pid;
    char name[256];
    long memory_kb;
    unsigned long long cpu_time;
    double cpu_usage;
} ProcessInfo;

int get_process_list(ProcessInfo processes[], int max_processes);

void sort_processes_by_memory(ProcessInfo processes[], int count);

void calculate_process_cpu_usage(
    ProcessInfo first[],
    int first_count,
    ProcessInfo second[],
    int second_count,
    unsigned long long total_cpu_diff
);
#endif

