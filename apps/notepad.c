#include "../include/notepad.h"
#include "../include/string.h"
#include <stddef.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

// Text buffer: each line is a fixed-size string
static char buffer[MAX_LINES][MAX_LINE_LENGTH];
static int line_count = 0;

// Cursor position (column = x, line = y)
static int cursor_x = 0, cursor_y = 0;

// Current file name
static char filename[256] = "";

// Prints an integer using only print() (no printf)
static void print_int(int num) {
    char temp[16];
    int i = 0;

    if (num == 0) {
        print("0");
        return;
    }

    // Convert number to string (reverse order)
    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }

    // Print digits in correct order
    while (i > 0) {
        char buf[2] = { temp[--i], '\0' };
        print(buf);
    }
}

// Draws the editor UI and visible text area
void notepad_display() {
    clear_screen();

    // Top border
    print("++----------------------------------------------------------++\n");
    print("| yoskOS Editor - ");
    print(filename[0] ? filename : "(Untitled)");

    // Right-align filename by adding spaces
    int name_len = strlen(filename[0] ? filename : "(Untitled)");
    for (int i = 0; i < 32 - name_len; i++) print(" ");

    print("| CTRL+S: Save | CTRL+X: Exit |\n");
    print("++----------------------------------------------------------++\n");
    print("\n");

    // Display visible lines (simple scrolling with cursor centered)
    int visible_lines = 18;
    int start_line = cursor_y - visible_lines / 2;

    if (start_line < 0) start_line = 0;
    if (start_line > line_count - visible_lines && line_count >= visible_lines)
        start_line = line_count - visible_lines;

    int i;
    for (i = start_line; i < start_line + visible_lines && i < line_count; i++) {
        // Line numbers: right-aligned, 3 digits (e.g. "  1:", " 10:", "100:")
        print(" ");
        if (i + 1 < 100) print(" ");
        if (i + 1 < 10)  print(" ");
        print_int(i + 1);
        print(": ");
        print(buffer[i]);
        print("\n");
    }

    // Fill remaining empty lines if file is short
    while (i < start_line + visible_lines) {
        print("   ~: \n");  // '~' represents empty lines
        i++;
    }

    // Bottom separator
    print("------------------------------------------------------------\n");

    // Status bar
    print("| Line: ");
    print_int(cursor_y + 1);
    print("  |  Column: ");
    print_int(cursor_x + 1);

    for (int j = 0; j < 28; j++) print(" ");
    print("|\n");

    print("------------------------------------------------------------\n");

    // Highlight active line and cursor
    if (line_count > 0) {
        print("> ");
        print(buffer[cursor_y]);
        print("\n  ");

        // Print characters up to cursor position
        int j;
        for (j = 0; j < cursor_x && j < MAX_LINE_LENGTH; j++) {
            char c = buffer[cursor_y][j];
            char buf[2] = { c ? c : ' ', '\0' };
            print(buf);
        }

        // Cursor indicator
        print("^");
        print("\n");
    }
}

// Loads a file into the editor buffer
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

    // Split file content into lines
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

    // Ensure at least one line exists
    if (line_count == 0) {
        buffer[0][0] = '\0';
        line_count = 1;
    }

    print("File loaded successfully.\n");
}

// Saves the current buffer to disk
void notepad_save() {
    if (filename[0] == '\0') {
        print("\nEnter file name: ");
        gets(filename);
    }

    char content[MAX_LINES * MAX_LINE_LENGTH] = {0};

    // Combine lines into a single string
    for (int i = 0; i < line_count; i++) {
        strcat(content, buffer[i]);
        if (i < line_count - 1 || buffer[i][strlen(buffer[i]) - 1] == '\n') {
            strcat(content, "\n");
        }
    }

    print("\nSaving file: ");
    print(filename);
    print("\n");

    if (fs_create(filename, content) == 0) {
        print("File saved successfully.\n");
    } else {
        print("Failed to save file!\n");
    }
}

// Handles keyboard input
void notepad_handle_input(int ch) {
    switch(ch) {
        case 0x11: // CTRL+Q
        case 0x18: // CTRL+X
            return;

        case 0x13: // CTRL+S
            notepad_save();
            break;

        case '\b': // Backspace
            if (cursor_x > 0) {
                memmove(&buffer[cursor_y][cursor_x - 1],
                        &buffer[cursor_y][cursor_x],
                        strlen(&buffer[cursor_y][cursor_x]) + 1);
                cursor_x--;
            }
            break;

        case '\n': // Enter - split line
            if (line_count >= MAX_LINES) break;

            // Store text after cursor
            char remainder[MAX_LINE_LENGTH];
            strcpy(remainder, &buffer[cursor_y][cursor_x]);
            buffer[cursor_y][cursor_x] = '\0';

            // Shift lines down
            if (cursor_y + 1 < line_count) {
                memmove(&buffer[cursor_y + 2],
                        &buffer[cursor_y + 1],
                        (line_count - cursor_y - 1) * MAX_LINE_LENGTH);
            }

            // Move remainder to new line
            strcpy(buffer[cursor_y + 1], remainder);

            line_count++;
            cursor_y++;
            cursor_x = 0;
            break;

        default:
            // Printable characters
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
}

// Editor entry point
int not_start(int argc, char *argv[]) {
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

    // Main editor loop
    while (1) {
        notepad_display();
        int ch = get_char();
        notepad_handle_input(ch);

        if (ch == 0x11 || ch == 0x18) break; // CTRL+Q or CTRL+X
    }

    return 0;
}
