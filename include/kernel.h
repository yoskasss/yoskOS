#ifndef KERNEL_H
#define KERNEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void clear_screen();
void print(const char* str);
void print_ascii_art(const char* text);

char get_char();
void gets(char* buf);

const char* fs_read(const char* name);
int fs_create(const char* name, const char* content);

size_t strlen(const char *str);
char* strcpy(char* dest, const char* src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char* s1, const char* s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);

int not_start(int argc, char *argv[]);

#endif