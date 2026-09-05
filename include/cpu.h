#ifndef CPU_H
#define CPU_H

typedef struct {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} CpuTimes;

CpuTimes get_cpu_times(void);

double calculate_cpu_usage(CpuTimes first, CpuTimes second);

unsigned long long get_total_cpu_time(CpuTimes times);

#endif
