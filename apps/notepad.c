#include "../include/notepad.h"
#include "../include/string.h"
#include <stddef.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

static char buffer[MAX_LINES][MAX_LINE_LENGTH];
static int line_count = 0;
static int cursor_x = 0, cursor_y = 0;
static char filename[256] = "";

void notepad_display() {
    clear_screen();
    
    print("yoskOS Editor - ");
    print(filename[0] ? filename : "(Isimsiz)");
    print(" | CTRL+S: Kaydet | CTRL+X: Cikis\n");
    print("------------------------------------------------------------\n");
    
    for (int i = 0; i < line_count; i++) {
        print(buffer[i]);
        if (i < line_count - 1 || buffer[i][strlen(buffer[i])-1] == '\n') {
            print("\n");
        }
    }
}

void notepad_load(const char *fname) {
    print("\nLoading file: ");
    print(fname);
    print("\n");

    const char* content = fs_read(fname);
    if (!content) {
        print("New file created.\n");
        strcpy(filename, fname);
        buffer[0][0] = '\0';
        line_count = 1;
        return;
    }

    strcpy(filename, fname);
    line_count = 0;
    
    const char *p = content;
    char *line = buffer[line_count];
    while (*p && line_count < MAX_LINES) {
        if (*p == '\n' || (line - buffer[line_count]) >= MAX_LINE_LENGTH - 1) {
            *line = '\0';
            line_count++;
            line = buffer[line_count];
            if (*p == '\n') p++;
        } else {
            *line++ = *p++;
        }
    }
    
    if (line_count == 0) {
        buffer[0][0] = '\0';
        line_count = 1;
    }

    print("File loaded successfully.\n");
}

void notepad_save() {
    if (filename[0] == '\0') {
        print("Dosya adi girin: ");
        char name[256];
        gets(name);
        strcpy(filename, name);
    }

    char content[MAX_LINES * MAX_LINE_LENGTH] = {0};
    for (int i = 0; i < line_count; i++) {
        strcat(content, buffer[i]);
        if (i < line_count - 1 || buffer[i][strlen(buffer[i])-1] == '\n') {
            strcat(content, "\n");
        }
    }

    print("\nSaving file: ");
    print(filename);
    print("\n");

    if (fs_create(filename, content) == 0) {
        print("Dosya kaydedildi.\n");
    } else {
        print("Dosya kaydedilemedi!\n");
    }
}

void notepad_handle_input(int ch) {
    switch(ch) {
        case 0x11: // CTRL+Q (çıkış)
        case 0x18: // CTRL+X (çıkış)
            return;
        case 0x13: // CTRL+S (kaydet)
            print("\nSaving file...\n"); // Debug print
            notepad_save();
            break;
        case '\b':
            if (cursor_x > 0) {
                memmove(&buffer[cursor_y][cursor_x-1],
                        &buffer[cursor_y][cursor_x],
                        strlen(&buffer[cursor_y][cursor_x]) + 1);
                cursor_x--;
            }
            break;
        case '\n':
            // Satır bölme işlemi
            break;
        default:
            if (ch >= 32 && ch <= 126) {
                if (strlen(buffer[cursor_y]) < MAX_LINE_LENGTH - 1) {
                    memmove(&buffer[cursor_y][cursor_x+1],
                            &buffer[cursor_y][cursor_x],
                            strlen(&buffer[cursor_y][cursor_x]) + 1);
                    buffer[cursor_y][cursor_x] = ch;
                    cursor_x++;
                }
            }
    }
}

int not_start(int argc, char *argv[]) {
    // Reset buffer and line count
    line_count = 0;
    cursor_x = 0;
    cursor_y = 0;
    filename[0] = '\0';

    if (argc > 1) {
        notepad_load(argv[1]);
    } else {
        buffer[0][0] = '\0';
        line_count = 1;
    }

    while (1) {
        notepad_display();
        int ch = get_char();
        notepad_handle_input(ch);
        if (ch == 0x11 || ch == 0x18) break; // Exit on Ctrl+Q or Ctrl+X
    }
    
    return 0;
}