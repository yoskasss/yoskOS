#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/info.h"
#include "../include/console.h"
#include "../include/string.h"

static inline void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    asm volatile("cpuid"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "a"(code));
}

void get_cpu_vendor(char* vendor) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, &eax, &ebx, &ecx, &edx);
    vendor[0] = (char)(ebx & 0xFF);
    vendor[1] = (char)((ebx >> 8) & 0xFF);
    vendor[2] = (char)((ebx >> 16) & 0xFF);
    vendor[3] = (char)((ebx >> 24) & 0xFF);
    vendor[4] = (char)(edx & 0xFF);
    vendor[5] = (char)((edx >> 8) & 0xFF);
    vendor[6] = (char)((edx >> 16) & 0xFF);
    vendor[7] = (char)((edx >> 24) & 0xFF);
    vendor[8] = (char)(ecx & 0xFF);
    vendor[9] = (char)((ecx >> 8) & 0xFF);
    vendor[10] = (char)((ecx >> 16) & 0xFF);
    vendor[11] = (char)((ecx >> 24) & 0xFF);
    vendor[12] = '\0';
}

void get_cpu_model(char* model) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    uint32_t model_id = (eax >> 4) & 0xF;
    uint32_t family_id = (eax >> 8) & 0xF;
    uint32_t stepping = eax & 0xF;

    model[0] = 'F'; model[1] = 'a'; model[2] = 'm'; model[3] = 'i'; model[4] = 'l'; model[5] = 'y'; model[6] = ' ';
    model[7] = '0' + family_id; model[8] = ','; model[9] = ' ';
    model[10] = 'M'; model[11] = 'o'; model[12] = 'd'; model[13] = 'e'; model[14] = 'l'; model[15] = ' ';
    model[16] = '0' + model_id; model[17] = ','; model[18] = ' ';
    model[19] = 'S'; model[20] = 't'; model[21] = 'e'; model[22] = 'p'; model[23] = 'p'; model[24] = 'i'; model[25] = 'n'; model[26] = 'g'; model[27] = ' ';
    model[28] = '0' + stepping; model[29] = '\0';
}

uint32_t get_memory_size() {

    return 128;
}

void show_info(const char* arg) {
    if (arg == NULL || strcmp(arg, "") == 0) {

        print_color("yoskOS Version 26.01.11\n", 0x09);
        print_color("A simple operating system developed in C.\n", 0x09);
    } else if (strcmp(arg, "-a") == 0) {

        print_color("yoskOS Version 26.01.11\n", 0x09);
        print_color("https://yoskasss.github.io/yoskOS\n", 0x09);
        print_color("This operating system is developed in C language.\n", 0x09);
        print_color("Features: Terminal, File System, Games, Notepad, Uptime\n", 0x09);
        print("\n");
        

        print_color("CPU Information:\n", 0x0B);
        char vendor[13];
        get_cpu_vendor(vendor);
        print("  Vendor: ");
        print(vendor);
        print("\n");
        
        char model[50];
        get_cpu_model(model);
        print("  Model: ");
        print(model);
        print("\n");
        print_color("  Architecture: x86 (32-bit)\n", 0x0B);
        print("\n");
        

        print_color("Memory Information:\n", 0x0C);
        uint32_t mem = get_memory_size();
        print("  Size: ");
        print_uint(mem);
        print(" MB\n");
        print_color("  Type: Physical memory management\n", 0x0C);
        print_color("  Features: Basic heap allocation\n", 0x0C);
        print("\n");
        

        print_color("Kernel Information:\n", 0x0D);
        print_color("  Type: Monolithic kernel\n", 0x0D);
        print_color("  Language: C\n", 0x0D);
        print_color("  Modules: Console, Games, File System, Apps\n", 0x0D);
    } else if (strcmp(arg, "cpu") == 0) {
        print_color("CPU Information:\n", 0x0B);
        char vendor[13];
        get_cpu_vendor(vendor);
        print("  Vendor: ");
        print(vendor);
        print("\n");
        
        char model[50];
        get_cpu_model(model);
        print("  Model: ");
        print(model);
        print("\n");
        
        print_color("  Architecture: x86 (32-bit)\n", 0x0B);
        print_color("  Features: Basic interrupt handling, timer\n", 0x0B);
    } else if (strcmp(arg, "mem") == 0) {
        print_color("Memory Information:\n", 0x0C);
        print_color("  Type: Physical memory management\n", 0x0C);
        uint32_t mem = get_memory_size();
        print("  Size: ");
        print_uint(mem);
        print(" MB\n");
        print_color("  Features: Basic heap allocation\n", 0x0C);
    } else if (strcmp(arg, "kernel") == 0) {
        print_color("Kernel Information:\n", 0x0D);
        print_color("  Type: Monolithic kernel\n", 0x0D);
        print_color("  Language: C\n", 0x0D);
        print_color("  Modules: Console, Games, File System, Apps\n", 0x0D);
    } else {
        print_color("Invalid info argument. Use: info, info -a, info cpu, info mem, info kernel\n", 0x0C);
    }
}