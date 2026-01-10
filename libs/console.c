#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/console.h"
#include "../include/string.h"

void print_char(char c) {
    VIDEO_MEMORY[cursor] = c | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
    cursor++;
    // Scroll if necessary
    if (cursor >= screen_width * screen_height) {
        // Move all lines up by one
        for (int i = 0; i < screen_width * (screen_height - 1); i++) {
            VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
        }
        // Clear the last line
        for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
            VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
        }
        cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
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

        // Handle newline, carriage return, backspace (single-byte characters)
        if (c == '\n') {
            cursor = ((cursor / screen_width) + 1) * screen_width; // Move to the start of the next line
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
            cursor = (cursor / screen_width) * screen_width; // Move to the start of the current line
            str++;
            continue;
        } else if (c == '\b') { // Backspace
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8); // Erase char with color
            }
            str++;
            continue;
        } else if (c < 128) {
            VIDEO_MEMORY[cursor++] = c | ((((bg_color << 4) | (color & 0x0F))) << 8); // Write char with color
            // Scroll if necessary
            if (cursor >= screen_width * screen_height) {
                // Move all lines up by one
                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                // Clear the last line
                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                }
                cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
            }
            str++;
            continue;
        } else {
            // Handle UTF-8 characters (simplified, assuming 2-byte sequences)
            if (c >= 0xC0) { // Start of multi-byte
                // For simplicity, print as '?' with color
                VIDEO_MEMORY[cursor++] = '?' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                str++; // Skip the byte
                if (*str) str++; // Skip next byte if exists
            } else {
                VIDEO_MEMORY[cursor++] = '?' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                str++;
            }
            // Scroll if necessary
            if (cursor >= screen_width * screen_height) {
                // Move all lines up by one
                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                // Clear the last line
                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((bg_color << 4) | (color & 0x0F))) << 8);
                }
                cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
            }
        }
    }
}

void print_ascii_art(const char* text) {
    int len = strlen(text);
    // Find max rows, assuming 5 for letters
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
                // For spaces or non-letters, print spaces for alignment
                print("     "); // 5 spaces
            }
            if (i < len - 1) print(" "); // space between letters
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
    while (input[i] && input[i] != ' ' && i < 19) { // Limit command length
        command[i] = input[i];
        i++;
    }
    command[i] = '\0';

    while (input[i] == ' ') i++;  // Skip spaces

    int j = 0;
    while (input[i] && j < 79) { // Limit arg length
        arg[j++] = input[i++];
    }
    arg[j] = '\0';
}

void get_line(char* buffer, int max_len) {
    int i = 0;

    while (i < max_len - 1) {
        char c = get_char();

        if (c == '\n' || c == '\r') { // Enter key
            print("\n");
            break;
        }
        if (c == '\b') { // Backspace
            if (i > 0) {
                i--;
                print("\b"); // Let print() handle the visual backspace
            }
            continue;
        }

        // Printable ASCII and ISO-8859-9 extended characters
        if (c != 0) { // Check if character is not null (e.g. from modifier keys)
            buffer[i++] = c;
            char s[2] = {c, '\0'};
            print(s);  // Echo character to screen
        }
    }
    buffer[i] = '\0';
}

// The main print function - this needs the full implementation from kernel.c
void print(const char* str) {
    if (screen_width == 0) screen_width = 80;
    if (screen_height == 0) screen_height = 24;
    while (*str) {
        unsigned char c = (unsigned char)*str;

        // Handle newline, carriage return, backspace (single-byte characters)
        if (c == '\n') {
            cursor = ((cursor / screen_width) + 1) * screen_width; // Move to the start of the next line
            // Scroll if necessary after newline
            if (cursor >= screen_width * screen_height) {
                // Move all lines up by one
                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                // Clear the last line
                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                }
                cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
            }
            str++;
            continue;
        } else if (c == '\r') {
            cursor = (cursor / screen_width) * screen_width; // Move to the start of the current line
            str++;
            continue;
        } else if (c == '\b') { // Backspace
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | ((((bg_color << 4) | (0x07 & 0x0F))) << 8); // Erase char with color
            }
            str++;
            continue;
        }

        // Handle ASCII characters (0-127)
        if (c < 128) {
            VIDEO_MEMORY[cursor++] = c | ((((0x07 & 0x0F) | (bg_color << 4)) << 8)); // Write char with default attribute
            str++;
            // Scroll if necessary (check after printing a character)
            if (cursor >= screen_width * screen_height) {
                // Move all lines up by one
                for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                // Clear the last line
                for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                    VIDEO_MEMORY[i] = ' ' | ((((0x07 & 0x0F) | (bg_color << 4)) << 8));
                }
                cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
            }
            continue;
        }

        // Handle common Turkish UTF-8 characters (2-byte sequences)
        if (c == 0xC3) { // Ö, Ü, ç, ö, ü, ğ
            str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?'; // Default placeholder
                switch (next_byte) {
                    case 0x96: display_char = '\xD6'; break; // C3 96 -> Ö (ISO-8859-9)
                    case 0x9C: display_char = '\xDC'; break; // C3 9C -> Ü (ISO-8859-9)
                    case 0xA7: display_char = '\xE7'; break; // C3 A7 -> ç (ISO-8859-9)
                    case 0xB6: display_char = '\xF6'; break; // C3 B6 -> ö (ISO-8859-9)
                    case 0xBC: display_char = '\xFC'; break; // C3 BC -> ü (ISO-8859-9)
                    // C3 A2 is â but not including it for simplicity
                }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
        } else if (c == 0xC4) { // Ğ, İ, ı, ğ
             str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?'; // Default placeholder
                 switch (next_byte) {
                    case 0x9E: display_char = '\xD0'; break; // C4 9E -> Ğ (ISO-8859-9)
                    case 0xB0: display_char = '\xDD'; break; // C4 B0 -> İ (ISO-8859-9)
                    case 0xB1: display_char = '\xEF'; break; // C4 B1 -> ı (ISO-8859-9)
                    case 0x9F: display_char = '\xF0'; break; // C4 9F -> ğ (ISO-8859-9)
                 }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                 VIDEO_MEMORY[cursor++] = '?' | (0x07 << 8);
            }
        } else if (c == 0xC5) { // Ş, ş
             str++;
            unsigned char next_byte = (unsigned char)*str;
            if (next_byte) {
                char display_char = '?'; // Default placeholder
                 switch (next_byte) {
                    case 0x9E: display_char = '\xA6'; break; // C5 9E -> Ş (ISO-8859-9)
                    case 0x9F: display_char = '\xB6'; break; // C5 9F -> ş (ISO-8859-9)
                 }
                 VIDEO_MEMORY[cursor++] = display_char | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
                 str++;
            } else {
                 VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
        } else { // Unhandled multi-byte start or invalid UTF-8
            VIDEO_MEMORY[cursor++] = '?' | (((0x07 & 0x0F) | (bg_color << 4)) << 8); // Print placeholder
            str++; // Skip the potentially invalid byte
        }

        // Scroll if necessary (check after printing a character)
        if (cursor >= screen_width * screen_height) {
            // Move all lines up by one
            for (int i = 0; i < screen_width * (screen_height - 1); i++) {
                VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
            }
            // Clear the last line
            for (int i = screen_width * (screen_height - 1); i < screen_width * screen_height; i++) {
                VIDEO_MEMORY[i] = ' ' | (((0x07 & 0x0F) | (bg_color << 4)) << 8);
            }
            cursor = screen_width * (screen_height - 1); // Set cursor to the beginning of the last line
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
    return -1; // Invalid color
}