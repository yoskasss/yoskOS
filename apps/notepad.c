#include "../include/notepad.h"
#include "../include/string.h"
#include "../include/console.h"
#include <stddef.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

static char buffer[MAX_LINES][MAX_LINE_LENGTH];
static int line_count = 0;

static int cursor_x = 0, cursor_y = 0;

static char filename[256] = "";
static int notepad_mode = 0; // 0 = GUI, 1 = Terminal

void notepad_display() {
    clear_screen();

    print("++----------------------------------------------------------++\n");
    print("| yoskOS Editor - ");
    print(filename[0] ? filename : "(Untitled)");

    int name_len = strlen(filename[0] ? filename : "(Untitled)");
    for (int i = 0; i < 32 - name_len; i++) print(" ");

    print("| CTRL+S: Save | CTRL+Q: Exit |\n");
    print("++----------------------------------------------------------++\n");
    print("\n");

    int visible_lines = 18;
    int start_line = cursor_y - visible_lines / 2;

    if (start_line < 0) start_line = 0;
    if (start_line > line_count - visible_lines && line_count >= visible_lines)
        start_line = line_count - visible_lines;

    int i;
    for (i = start_line; i < start_line + visible_lines && i < line_count; i++) {

        print(" ");
        if (i + 1 < 100) print(" ");
        if (i + 1 < 10)  print(" ");
        print_int(i + 1);
        print(": ");
        print(buffer[i]);
        print("\n");
    }

    while (i < start_line + visible_lines) {
        print("   ~: \n");
        i++;
    }

    print("------------------------------------------------------------\n");

    print("| Line: ");
    print_int(cursor_y + 1);
    print("  |  Column: ");
    print_int(cursor_x + 1);

    for (int j = 0; j < 28; j++) print(" ");
    print("|\n");

    print("------------------------------------------------------------\n");

    if (line_count > 0) {
        print("> ");
        print(buffer[cursor_y]);
        print("\n  ");

        int j;
        for (j = 0; j < cursor_x && j < MAX_LINE_LENGTH; j++) {
            char c = buffer[cursor_y][j];
            char buf[2] = { c ? c : ' ', '\0' };
            print(buf);
        }

        print("^");
        print("\n");
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
        strcpy(filename, "untitled.txt");
    }

    fs_create(filename, buffer[0]);
}

int notepad_handle_input(int ch) {
    switch(ch) {
        case 0x11:
            return 2;  // 2 = exit request

        case 0x18:
            return 2;  // 2 = exit request

        case 0x13:
            notepad_save();
            break;

        case '\b':
            if (cursor_x > 0) {
                memmove(&buffer[cursor_y][cursor_x - 1],
                        &buffer[cursor_y][cursor_x],
                        strlen(&buffer[cursor_y][cursor_x]) + 1);
                cursor_x--;
            }
            break;

        case '\n':
            if (line_count >= MAX_LINES) break;

            char remainder[MAX_LINE_LENGTH];
            strcpy(remainder, &buffer[cursor_y][cursor_x]);
            buffer[cursor_y][cursor_x] = '\0';

            if (cursor_y + 1 < line_count) {
                memmove(&buffer[cursor_y + 2],
                        &buffer[cursor_y + 1],
                        (line_count - cursor_y - 1) * MAX_LINE_LENGTH);
            }

            strcpy(buffer[cursor_y + 1], remainder);

            line_count++;
            cursor_y++;
            cursor_x = 0;
            break;

        default:

            if (ch >= 32 && ch <= 126) {
                if (strlen(buffer[cursor_y]) < MAX_LINE_LENGTH - 1) {
                    memmove(&buffer[cursor_y][cursor_x + 1],
                            &buffer[cursor_y][cursor_x],
                            strlen(&buffer[cursor_y][cursor_x]) + 1);
                    buffer[cursor_y][cursor_x] = ch;
                    cursor_x++;
                }
            }
            break;
    }
    return 0;
}

int not_start(int argc, char *argv[], int mode) {
    line_count = 0;
    cursor_x = 0;
    cursor_y = 0;
    filename[0] = '\0';
    notepad_mode = mode;

    if (argc > 1) {
        notepad_load(argv[1]);
    } else {
        buffer[0][0] = '\0';
        line_count = 1;
    }

    notepad_display();

    while (1) {
        int ch = get_char();
        
        if (ch == 0x14) {
            mini_terminal();
            notepad_display();
            continue;
        }
        
        if (ch != 0) {
            int result = notepad_handle_input(ch);
            
            if (result == 2) {
                // Exit request - ask user to save
                print("\n");
                print("========================================\n");
                print("Save file before exiting? (y/n): ");
                
                // Wait for user response
                int response = 0;
                while (response == 0) {
                    response = get_char();
                }
                
                // Echo the response
                char response_str[2] = {response, '\0'};
                print(response_str);
                print("\n");
                
                if (response == 'y' || response == 'Y') {
                    notepad_save();
                }
                
                // Exit application
                return 0;
            }
            
            notepad_display();
        }
    }

    return 0;
}

void notepad_entry() {
    char *argv[] = {"not", "task.txt"};
    not_start(2, argv, 0);
}
