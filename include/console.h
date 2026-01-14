#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stdbool.h>

void print(const char* str);
void print_char(char c);
void print_int(int n);
void print_color_int(int n, uint8_t color);
void print_uint(uint64_t num);
void print_color(const char* str, uint8_t color);
void print_ascii_art(const char* text);
void clear_screen();
void parse_input(const char* input, char* command, char* arg);
void get_line(char* buffer, int max_len);
int get_color_code(const char* color_name);

extern volatile unsigned short* VIDEO_MEMORY;
extern int cursor;
extern int screen_width;
extern int screen_height;
extern int bg_color;
extern const char* ascii_letters[26][5];
extern char tr_q_keymap[128];
extern bool shift_active;
extern bool caps_lock_active;
extern bool ctrl_active;
extern char username[32];
extern char log_buffer[4096];

#endif