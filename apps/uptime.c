#include <stdint.h>
#include "../include/uptime.h"
#include "../include/console.h"

extern void putchar(char c);
extern void puts(const char* str);
extern uint16_t* vga_buffer;
extern uint16_t cursor_pos;

volatile uint64_t start_tsc = 0;

static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void start_uptime_timer() {
    start_tsc = rdtsc();
}

double get_uptime_seconds(uint64_t cpu_freq_hz) {
    uint64_t current = rdtsc();
    uint64_t cycles_elapsed = current - start_tsc;
    return (double)cycles_elapsed / cpu_freq_hz;
}

void print_uptime(uint64_t cpu_freq_hz) {
    double uptime = get_uptime_seconds(cpu_freq_hz);
    uint64_t total_seconds = (uint64_t)uptime;

    uint64_t h = total_seconds / 3600;
    uint64_t m = (total_seconds % 3600) / 60;
    uint64_t s = total_seconds % 60;

    print_uint(h); puts(" hours, ");
    print_uint(m); puts(" minutes, ");
    print_uint(s); puts(" seconds\n");
}