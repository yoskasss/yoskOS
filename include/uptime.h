#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

void start_uptime_timer(void);
void print_uptime(uint64_t cpu_freq_hz);

#endif