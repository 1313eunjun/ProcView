#include <stdio.h>

#include "cpu.h"

CpuTimes get_cpu_times(void)
{
    FILE *file = fopen("/proc/stat", "r");

    CpuTimes times = {0};

    if (file == NULL) {
        perror("Failed to open /proc/stat");
        return times;
    }

    fscanf(file,
           "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &times.user,
           &times.nice,
           &times.system,
           &times.idle,
           &times.iowait,
           &times.irq,
           &times.softirq,
           &times.steal);

    fclose(file);

    return times;
}


double calculate_cpu_usage(CpuTimes first, CpuTimes second)
{
    unsigned long long first_idle =
        first.idle + first.iowait;

    unsigned long long second_idle =
        second.idle + second.iowait;

    unsigned long long first_total =
        first.user +
        first.nice +
        first.system +
        first.idle +
        first.iowait +
        first.irq +
        first.softirq +
        first.steal;

    unsigned long long second_total =
        second.user +
        second.nice +
        second.system +
        second.idle +
        second.iowait +
        second.irq +
        second.softirq +
        second.steal;

    unsigned long long total_diff =
        second_total - first_total;

    unsigned long long idle_diff =
        second_idle - first_idle;

    if (total_diff == 0) {
        return 0.0;
    }

    return 100.0 * (total_diff - idle_diff) / total_diff;
}


unsigned long long get_total_cpu_time(CpuTimes times)
{
    return times.user +
           times.nice +
           times.system +
           times.idle +
           times.iowait +
           times.irq +
           times.softirq +
           times.steal;
}
