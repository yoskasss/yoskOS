#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/console.h"
#include "../include/string.h"
#include "../include/process.h"

void print_char(char c) {
    VIDEO_MEMORY[cursor] = c | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
    cursor++;

    if (cursor >= screen_width * screen_height) {

        for (int i = 0; i < screen_width * (screen_height - 1); i++) {
            VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
        }

        for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
            VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
        }
        cursor = screen_width * (screen_height - 1);
    }
}

void print_int(int n) {
    if (n < 0) {
        print_char('-');
        n = -n;
    }

    if (n == 0) {
        print_char('0');
        return;
    }
    
    if (n / 10) {
        print_int(n / 10);
    }
    print_char((n % 10) + '0');
}

void print_color_int(int n, uint8_t color) {
    if (n < 0) {
        print_color("-", color);
        n = -n;
    }

    char buffer[32] = {0};
    int i = 0;

    if (n == 0) {
        buffer[0] = '0';
        i = 1;
    } else {
        while (n > 0) {
            buffer[i++] = (n % 10) + '0';
            n /= 10;
        }
    }

    for (int j = i - 1; j >= 0; j--) {
        char c[2] = {buffer[j], '\0'};
        print_color(c, color);
    }
}

void print_uint(uint64_t num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    if (num / 10) {
        print_uint(num / 10);
    }
    print_char((num % 10) + '0');
}

void print_color(const char* str, uint8_t color) {
    if (screen_width == 0) screen_width = 80;
    if (screen_height == 0) screen_height = 24;
    while (*str) {
        unsigned char c = (unsigned char)*str;

        if (c == '\n') {
            cursor = ((cursor / screen_width) + 1) * screen_width;
            if (cursor >= screen_width * screen_height) {
                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                }
                cursor = screen_width * (screen_height - 1);
            }
            str++;
            continue;
        } else if (c == '\r') {
            cursor = (cursor / screen_width) * screen_width;
            str++;
            continue;
        } else if (c == '\b') {
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
            }
            str++;
            continue;
        } else if (c < 128) {
            VIDEO_MEMORY[cursor++] = c | ((((bg_color << 4) | (color & 0x0F))) << 8);

            if (cursor >= screen_width * screen_height) {

                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }

                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                }
                cursor = screen_width * (screen_height - 1);
            }
            str++;
            continue;
        } else {

            if (c >= 0xC0) {

                VIDEO_MEMORY[cursor++] = '?' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                str++;
                if (*str) str++;
            } else {
                VIDEO_MEMORY[cursor++] = '?' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                str++;
            }

            if (cursor >= screen_width * screen_height) {

                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }

                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                }
                cursor = screen_width * (screen_height - 1);
            }
        }
    }
}

void print_ascii_art(const char* text) {
    int len = strlen(text);

    int max_rows = 5;
    for (int row = 0; row < max_rows; row++) {
        for (int i = 0; i < len; i++) {
            char c = text[i];
            if (c >= 'A' && c <= 'Z') {
                int index = c - 'A';
                print(ascii_letters[index][row]);
            } else if (c >= 'a' && c <= 'z') {
                int index = c - 'a';
                print(ascii_letters[index][row]);
            } else {

                print("     ");
            }
            if (i < len - 1) print(" ");
        }
        print("\n");
    }
}

void clear_screen() {
    for (int i = 0; i < screen_width * screen_height; i++) {
        VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
    }
    cursor = 0;
}

void parse_input(const char* input, char* command, char* arg) {
    int i = 0;
    while (input[i] && input[i] != ' ' && i < 19) {
        command[i] = input[i];
        i++;
    }
    command[i] = '\0';

    while (input[i] == ' ') i++;

    int j = 0;
    while (input[i] && j < 79) {
        arg[j++] = input[i++];
    }
    arg[j] = '\0';
}

void get_line(char* buffer, int max_len) {
    int i = 0;

    while (i < max_len - 1) {
        char c = get_char();

        if (c == 0) {
            process_yield();
            continue;
        }

        if (c == 0x14) {
            mini_terminal();
            buffer[i] = '\0';
            return;
        }

        if (c == '\n' || c == '\r') {
            print("\n");
            break;
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                print("\b");
            }
            continue;
        }

        if (c != 0) {
            buffer[i++] = c;
            char s[2] = {c, '\0'};
            print(s);
        }
    }
    buffer[i] = '\0';
}

void print(const char* str) {
    if (screen_width == 0) screen_width = 80;
    if (screen_height == 0) screen_height = 24;
    while (*str) {
        unsigned char c = (unsigned char)*str;

        if (c == '\n') {
            cursor = ((cursor / screen_width) + 1) * screen_width;

            if (cursor >= screen_width * screen_height) {

                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }

                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                }
                cursor = screen_width * (screen_height - 1);
            }
            str++;
            continue;
        } else if (c == '\r') {
            cursor = (cursor / screen_width) * screen_width;
            str++;
            continue;
        } else if (c == '\b') {
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | ((((bg_color << 4) | (0x07 & 0x0F))) << 8);
            }
            str++;
            continue;
        }

        if (c < 128) {
            VIDEO_MEMORY[cursor++] = c | ((((0x07 & 0x0F) | (bg_color << 4)) << 8));
            str++;

            if (cursor >= screen_width * screen_height) {

                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }

                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((0x07 & 0x0F) | (bg_color << 4)) << 8));
                }
                cursor = screen_width * (screen_height - 1);
            }
            continue;
        }

        if (c == 0xC3) {
            str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?';
                switch (next_byte) {
                    case 0x96: display_char = '\xD6'; break;
                    case 0x9C: display_char = '\xDC'; break;
                    case 0xA7: display_char = '\xE7'; break;
                    case 0xB6: display_char = '\xF6'; break;
                    case 0xBC: display_char = '\xFC'; break;

                }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
        } else if (c == 0xC4) {
             str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?';
                 switch (next_byte) {
                    case 0x9E: display_char = '\xD0'; break;
                    case 0xB0: display_char = '\xDD'; break;
                    case 0xB1: display_char = '\xEF'; break;
                    case 0x9F: display_char = '\xF0'; break;
                 }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                 VIDEO_MEMORY[cursor++] = '?' | (0x07 << 8);
            }
        } else if (c == 0xC5) {
             str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?';
                 switch (next_byte) {
                    case 0x9E: display_char = '\xA6'; break;
                    case 0x9F: display_char = '\xB6'; break;
                 }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                 VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
        } else {
            VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            str++;
        }

        if (cursor >= screen_width * screen_height) {

            for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
            }

            for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
            cursor = screen_width * (screen_height - 1);
        }
    }
}

int get_color_code(const char* color_name) {
    if (strcmp(color_name, "black") == 0) return 0;
    if (strcmp(color_name, "blue") == 0) return 1;
    if (strcmp(color_name, "green") == 0) return 2;
    if (strcmp(color_name, "cyan") == 0) return 3;
    if (strcmp(color_name, "red") == 0) return 4;
    if (strcmp(color_name, "magenta") == 0) return 5;
    if (strcmp(color_name, "brown") == 0) return 6;
    if (strcmp(color_name, "lightgray") == 0) return 7;
    if (strcmp(color_name, "darkgray") == 0) return 8;
    if (strcmp(color_name, "lightblue") == 0) return 9;
    if (strcmp(color_name, "lightgreen") == 0) return 10;
    if (strcmp(color_name, "lightcyan") == 0) return 11;
    if (strcmp(color_name, "lightred") == 0) return 12;
    if (strcmp(color_name, "lightmagenta") == 0) return 13;
    if (strcmp(color_name, "yellow") == 0) return 14;
    if (strcmp(color_name, "white") == 0) return 15;
    return -1;
}