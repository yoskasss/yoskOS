#include <stdbool.h> 
#include <stddef.h> 
#include <stdint.h>
#include "../include/string.h"  
#include "../include/notepad.h" 
#include "../include/uptime.h"  
#include "../include/images.h"  
#include "../include/console.h"  
#include "../include/info.h"  

volatile unsigned int timer_ticks = 0;

char log_buffer[4096] = "";

int screen_width = 80;
int screen_height = 24;
int bg_color = 0;

char* strcpy(char* dest, const char* src);
int not_start(int argc, char *argv[]);
void gets(char* buf);

double get_uptime_seconds(uint64_t cpu_freq_hz);
const char* strchr(const char* str, int c);

#define MAX_FILES 32
#define MAX_FILENAME 32
#define MAX_FILECONTENT 1024

typedef struct {
    char name[MAX_FILENAME];
    char content[MAX_FILECONTENT];
    bool used;
} File;

File fs[MAX_FILES];

bool shift_active = false;
bool caps_lock_active = false;
bool ctrl_active = false;

volatile unsigned short* VIDEO_MEMORY = (unsigned short*) 0xB8000;
int cursor = 0;
char username[32];
const char* ascii_letters[26][5] = {
    {
        "  A  ",
        " A A ",
        "AAAAA",
        "A   A",
        "A   A"
    },
    {
        "BBBB ",
        "B   B",
        "BBBB ",
        "B   B",
        "BBBB "
    },
    {
        " CCCC",
        "C    ",
        "C    ",
        "C    ",
        " CCCC"
    },
    {
        "DDDD ",
        "D   D",
        "D   D",
        "D   D",
        "DDDD "
    },
    // E
    {
        "EEEEE",
        "E    ",
        "EEE  ",
        "E    ",
        "EEEEE"
    },
    // F
    {
        "FFFFF",
        "F    ",
        "FFF  ",
        "F    ",
        "F    "
    },
    // G
    {
        "GGGGG",
        "G    ",
        "G GGG",
        "G   G",
        " GGG "
    },
    // H
    {
        "H   H",
        "H   H",
        "HHHHH",
        "H   H",
        "H   H"
    },
    // I
    {
        "IIIII",
        "  I  ",
        "  I  ",
        "  I  ",
        "IIIII"
    },
    // J
    {
        "JJJJJ",
        "    J",
        "    J",
        "J   J",
        " JJJ "
    },
    // K
    {
        "K   K",
        "K  K ",
        "KKK  ",
        "K  K ",
        "K   K"
    },
    // L
    {
        "L    ",
        "L    ",
        "L    ",
        "L    ",
        "LLLLL"
    },
    // M
    {
        "M   M",
        "MM MM",
        "M M M",
        "M   M",
        "M   M"
    },
    // N
    {
        "N   N",
        "NN  N",
        "N N N",
        "N  NN",
        "N   N"
    },
    // O
    {
        " OOO ",
        "O   O",
        "O   O",
        "O   O",
        " OOO "
    },
    // P
    {
        "PPPP ",
        "P   P",
        "PPPP ",
        "P    ",
        "P    "
    },
    // Q
    {
        " QQQ ",
        "Q   Q",
        "Q Q Q",
        "Q  Q ",
        " QQ Q"
    },
    // R
    {
        "RRRR ",
        "R   R",
        "RRRR ",
        "R  R ",
        "R   R"
    },
    // S
    {
        " SSSS",
        "S    ",
        " SSS ",
        "    S",
        "SSSS "
    },
    // T
    {
        "TTTTT",
        "  T  ",
        "  T  ",
        "  T  ",
        "  T  "
    },
    // U
    {
        "U   U",
        "U   U",
        "U   U",
        "U   U",
        " UUU "
    },
    // V
    {
        "V   V",
        "V   V",
        "V   V",
        " V V ",
        "  V  "
    },
    // W
    {
        "W   W",
        "W   W",
        "W W W",
        "WW WW",
        "W   W"
    },
    // X
    {
        "X   X",
        " X X ",
        "  X  ",
        " X X ",
        "X   X"
    },
    // Y
    {
        "Y   Y",
        " Y Y ",
        "  Y  ",
        "  Y  ",
        "  Y  "
    },
    // Z
    {
        "ZZZZZ",
        "   Z ",
        "  Z  ",
        " Z   ",
        "ZZZZZ"
    }
};

// These characters are set according to ISO-8859-9 (Latin-5) encoding.

char tr_q_keymap[128] = {
    // Scancode (Hex): 0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E
    /* 00-14 */ 0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '-', '\b',
    // ^ (null)  ^Esc   ^1    ^2    ^3    ^4    ^5    ^6    ^7    ^8    ^9    ^0    ^*    ^-    ^Backspace

    // Scancode (Hex): 0x0F  0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17  0x18  0x19  0x1A  0x1B  0x1C  0x1D
    /* 15-29 */ '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\xF0', '\xFC', '\n', 0,
    // ^Tab  ^Q    ^W    ^E    ^R    ^T    ^Y    ^U    ^I    ^O    ^P    ^ğ    ^ü    ^Enter ^Left Ctrl
    // '\xF0' for 'ğ', '\xFC' for 'ü' (ISO-8859-9)

    // Scancode (Hex): 0x1E  0x1F  0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27  0x28  0x29  0x2A  0x2B  0x2C
    /* 30-44 */ 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\xB6', '\xEF', ',', 0,   '<', 'z',
    // ^A    ^S    ^D    ^F    ^G    ^H    ^J    ^K    ^L    ^ş    ^ı    ^,    ^Left Shift ^<    ^Z
    // '\xB6' for 'ş', '\xEF' for 'ı' (ISO-8859-9)

    // Scancode (Hex): 0x2D  0x2E  0x2F  0x30  0x31  0x32  0x33  0x34  0x35  0x36  0x37  0x38  0x39
    /* 45-57 */ 'x', 'c', 'v', 'b', 'n', 'm', '\xF6', '\xE7', '.', 0,   '*', 0,   ' ',
    // ^X    ^C    ^V    ^B    ^N    ^M    ^ö    ^ç    ^.    ^Right Shift ^Numpad * ^Left Alt ^Space
    // '\xF6' for 'ö', '\xE7' for 'ç' (ISO-8859-9)

    // Scancode (Hex): 0x3A  0x3B  0x3C  0x3D  0x3E  0x3F  0x40  0x41  0x42  0x43  0x44  0x45  0x46  0x47  0x48
    /* 58-72 */ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '7', '8',
    // ^Caps Lock ^F1   ^F2   ^F3   ^F4   ^F5   ^F6   ^F7   ^F8   ^F9   ^F10  ^Num Lock ^Scroll Lock ^Numpad 7 ^Numpad 8

    // Scancode (Hex): 0x49  0x4A  0x4B  0x4C  0x4D  0x4E  0x4F  0x50  0x51  0x52  0x53  0x54  0x55  0x56  0x57
    /* 73-87 */ '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0,   0,   0,   0,
    // ^Numpad 9 ^Numpad - ^Numpad 4 ^Numpad 5 ^Numpad 6 ^Numpad + ^Numpad 1 ^Numpad 2 ^Numpad 3 ^Numpad 0 ^Numpad . ^SysRq  ^<unmapped> ^<unmapped> ^F11

    // Scancode (Hex): 0x58  0x59  0x5A  0x5B  0x5C  0x5D  0x5E  0x5F  0x60  0x61  0x62  0x63  0x64  0x65  0x66
    /* 88-102*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    // ^F12  ^<unmapped> ... (Commonly Right Shift, Right Ctrl, Right Alt, Home, Up, PgUp, Left, Right, End, Down, PgDn, Insert, Delete, etc. - These often require extended scancodes or are handled as control/action keys, not character producers.)

    // Scancode (Hex): 0x67  0x68  0x69  0x6A  0x6B  0x6C  0x6D  0x6E  0x6F  0x70  0x71  0x72  0x73  0x74  0x75
    /* 103-117*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,

    // Scancode (Hex): 0x76  0x77  0x78  0x79  0x7A  0x7B  0x7C  0x7D  0x7E  0x7F
    /* 118-127*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0
};

// Scancode → character map (shifted) for Turkish Q keyboard
char tr_q_keymap_shifted[128] = {
    // Scancode (Hex): 0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E
    /* 00-14 */ 0,    27,  '!', '\'', '^', '+', '%', '&', '/', '(', ')', '=', '?', '_', '\b',
    // ^ (null)  ^Esc   ^!    ^'    ^^    ^+    ^%    ^&    ^/    ^(    ^)    ^=    ^?    ^_    ^Backspace

    // Scancode (Hex): 0x0F  0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17  0x18  0x19  0x1A  0x1B  0x1C  0x1D
    /* 15-29 */ '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '\xD0', '\xDC', '\n', 0,
    // ^Tab  ^Q    ^W    ^E    ^R    ^T    ^Y    ^U    ^I    ^O    ^P    ^Ğ    ^Ü    ^Enter ^Left Ctrl
    // '\xD0' for 'Ğ', '\xDC' for 'Ü' (ISO-8859-9)

    // Scancode (Hex): 0x1E  0x1F  0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27  0x28  0x29  0x2A  0x2B  0x2C
    /* 30-44 */ 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\xA6', '\xDD', ';', 0,   '>', 'Z',
    // ^A    ^S    ^D    ^F    ^G    ^H    ^J    ^K    ^L    ^Ş    ^İ    ^;    ^Left Shift ^>    ^Z
    // '\xA6' for 'Ş', '\xDD' for 'İ' (ISO-8859-9)

    // Scancode (Hex): 0x2D  0x2E  0x2F  0x30  0x31  0x32  0x33  0x34  0x35  0x36  0x37  0x38  0x39
    /* 45-57 */ 'X', 'C', 'V', 'B', 'N', 'M', '\xD6', '\xC7', ':', 0,   '*', 0,   ' ',
    // ^X    ^C    ^V    ^B    ^N    ^M    ^Ö    ^Ç    ^:    ^Right Shift ^Numpad * ^Left Alt ^Space
    // '\xD6' for 'Ö', '\xC7' for 'Ç' (ISO-8859-9)

    // Scancode (Hex): 0x3A-0x58 (Control keys, F-keys, Numpad digits are generally not shifted)
    /* 58-72 */ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '7', '8',
    /* 73-87 */ '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0,   0,   0,   0,
    /* 88-102*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 103-117*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    /* 118-127*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,   0
};

// ------------ VGA Graphics Functions -------------

#define VGA_TEXT_WIDTH  80
#define VGA_TEXT_HEIGHT 25
#define VGA_ADDR   0xB8000

/* ------------------ VGA ------------------ */

void vga_put_char_at(char c, int x, int y, unsigned char color) {
    volatile unsigned short* vga = (unsigned short*)VGA_ADDR;
    vga[y * VGA_TEXT_WIDTH + x] = (color << 8) | c;
}

void vga_print_at(const char* str, int x, int y, unsigned char color) {
    for (int i = 0; str[i]; i++) {
        vga_put_char_at(str[i], x + i, y, color);
    }
}

void clear_line(int y) {
    for (int x = 0; x < VGA_TEXT_WIDTH; x++) {
        vga_put_char_at(' ', x, y, 0x07);
    }
}

void scroll_up() {
    for (int i = 0; i < 1920; i++) { // 80*24 = 1920
        VIDEO_MEMORY[i] = VIDEO_MEMORY[i + 80];
    }

    for (int i = 1920; i < 2000; i++) {
        VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
    }

    if (cursor >= 1920) {
        cursor = 1920;
    }
}

char history_line[81];

void scroll_down() {
    // Save the first line to history
    for (int i = 0; i < 80; i++) {
        history_line[i] = VIDEO_MEMORY[i] & 0xFF; 
    }
    history_line[80] = '\0';

    for (int i = 1999; i >= 80; i--) {
        VIDEO_MEMORY[i] = VIDEO_MEMORY[i - 80];
    }
    // Clear the first line
    for (int i = 0; i < 80; i++) {
        VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
    }
    // Adjust cursor if necessary
    if (cursor < 80) {
        cursor = 80;
    }
}


/* ------------------ DEBUG BAR ------------------ */

void draw_toolbar(const char* user, const char* path) {
    clear_line(0);

    // User
    vga_print_at("User: ", 0, 0, 0x0D);
    vga_print_at(user, 6, 0, 0x0D);

    // Path
    int path_start = 6 + strlen(user) + 2;
    vga_print_at("Path: ", path_start, 0, 0x0D);
    vga_print_at(path, path_start + 6, 0, 0x0D);

    // Uptime
    double uptime = get_uptime_seconds(1000000000); // Assuming 1GHz
    int total_seconds = (int)uptime;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    char uptime_str[20];
    int idx = 0;
    if (minutes >= 10) uptime_str[idx++] = '0' + minutes / 10;
    uptime_str[idx++] = '0' + minutes % 10;
    uptime_str[idx++] = 'm';
    uptime_str[idx++] = ' ';
    if (seconds >= 10) uptime_str[idx++] = '0' + seconds / 10;
    uptime_str[idx++] = '0' + seconds % 10;
    uptime_str[idx++] = 's';
    uptime_str[idx] = '\0';
    int uptime_start = path_start + 6 + strlen(path) + 2;
    vga_print_at("Uptime: ", uptime_start, 0, 0x0D);
    vga_print_at(uptime_str, uptime_start + 8, 0, 0x0D);
}
int ticks = 0;
int state = 3;





#define VGA_GRAPHICS_WIDTH 320
#define VGA_GRAPHICS_HEIGHT 200
#define TEXT_MODE 0x03
#define GRAPHICS_MODE 0x13

// Assembly function to set the VGA mode
void set_vga_mode(int mode) {
    (void)mode;
}

void draw_pixel(int x, int y, unsigned char color) {
    if (x < 0 || x >= VGA_GRAPHICS_WIDTH || y < 0 || y >= VGA_GRAPHICS_HEIGHT) {
        return;
    }
    unsigned char* vga_memory = (unsigned char*) 0xA0000;
    vga_memory[y * VGA_GRAPHICS_WIDTH + x] = color;
}

void draw_bitmap(const unsigned char* data, int x, int y, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, data[i * width + j]);
        }
    }
}

void clear_graphics_screen(unsigned char color) {
    unsigned char* vga_memory = (unsigned char*) 0xA0000;
    for (int i = 0; i < VGA_GRAPHICS_WIDTH * VGA_GRAPHICS_HEIGHT; i++) {
        vga_memory[i] = color;
    }
}

// ----------------- Hardware Level Functions ------------------

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// ----------------- Helper Functions --------------------------
void timer_handler() {
    timer_ticks++;
}
void timer_init() {
    unsigned int divisor = 1193180 / 100;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

int simple_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + str[i] - '0';
        } else {
            break;
        }
    }
    return res;
}
#define MAX_NAME_LEN 32
#define MAX_CHILDREN 16
#define MAX_FOLDERS 128

typedef struct Folder {
    char name[MAX_NAME_LEN];
    struct Folder* parent;
    struct Folder* children[MAX_CHILDREN];
    int child_count;
} Folder;

Folder folder_pool[MAX_FOLDERS];
int folder_pool_index = 0;


Folder* allocate_folder() {
    if (folder_pool_index >= MAX_FOLDERS)
        return NULL;
    return &folder_pool[folder_pool_index++];
}

Folder* create_folder(const char* name, Folder* parent) {
    if (!parent || parent->child_count >= MAX_CHILDREN)
        return NULL;

    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, name) == 0)
            return NULL; 
    }

    Folder* new_folder = allocate_folder();
    if (!new_folder) return NULL;

    strncpy(new_folder->name, name, MAX_NAME_LEN);
    new_folder->name[MAX_NAME_LEN - 1] = '\0'; 
    new_folder->parent = parent;
    new_folder->child_count = 0;

    parent->children[parent->child_count++] = new_folder;
    return new_folder;
}

void list_folder(Folder* folder) {
    if (folder->child_count == 0) {
        print("Folder is empty.\n");
        return;
    }
    for (int i = 0; i < folder->child_count; i++) {
        print(folder->children[i]->name);
        print("  ");
    }
    print("\n");
}

void print_path(Folder* folder) {
    if (folder->parent != NULL) {
        print_path(folder->parent);
    } else {
        print("");
    }
    print("/");
    print(folder->name);
}

char* get_path_string(Folder* folder, char* buffer, int max_len) {
    if (folder->parent == NULL) {
        buffer[0] = '/';
        strcpy(buffer + 1, folder->name);
    } else {
        get_path_string(folder->parent, buffer, max_len);
        int len = strlen(buffer);
        if (len + 1 + strlen(folder->name) < max_len) {
            buffer[len] = '/';
            buffer[len+1] = '\0';
            strcpy(buffer + len + 1, folder->name);
        }
    }
    return buffer;
}

Folder* find_subfolder(Folder* current, const char* name) {
    for (int i = 0; i < current->child_count; i++) {
        if (strcmp(current->children[i]->name, name) == 0)
            return current->children[i];
    }
    return NULL;
}

void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        fs[i].used = false;
    }
}

int fs_create(const char* name, const char* content) {
    print("\nTrying to create file: ");
    print(name);
    print("\n");
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used && strcmp(fs[i].name, name) == 0) {
            strncpy(fs[i].content, content, MAX_FILECONTENT - 1);
            fs[i].content[MAX_FILECONTENT - 1] = '\0';
            print("File updated successfully!\n");
            return 0;
        }
    }
    
    // Create new file
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs[i].used) {
            fs[i].used = true;
            strncpy(fs[i].name, name, MAX_FILENAME - 1);
            fs[i].name[MAX_FILENAME - 1] = '\0';

            strncpy(fs[i].content, content, MAX_FILECONTENT - 1);
            fs[i].content[MAX_FILECONTENT - 1] = '\0';

            print("File created successfully!\n");
            return 0;
        }
    }
    print("No space left in filesystem!\n");
    return -1;
}

void fs_list() {
    bool found_file = false;
    print("\nFiles in system:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used) {
            print("- ");
            print(fs[i].name);
            print("\n");
            found_file = true;
        }
    }
    if (!found_file) {
        print("No files found.\n");
    }
}

const char* fs_read(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used && strcmp(fs[i].name, name) == 0) {
            return fs[i].content;
        }
    }
    return NULL;
}
void qemu_exit() {
    asm volatile (
        "mov $0x2000, %ax\n"
        "mov $0x604, %dx\n"
        "out %ax, %dx\n"
    );
}
void reboot() {
    unsigned char good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}


void kernel_exit() {
    asm volatile ("cli"); // disable interrupts
    while (1) {
        asm volatile ("hlt"); // halt CPU
    }
}



// We downscale the image by grouping blocks of pixels to fit the 80x25 text screen.
void render_bitmap_ascii(const unsigned char* data, int width, int height) {
    // ASCII ramp from dark -> light
    const char* ramp = " @%#*+=-:. "; // 10 levels (space = darkest)
    const int ramp_len = 10;

    // Choose block size to fit in 80x25 text terminal.
    // For typical 64x64 bitmap, block_w = 2, block_h = 3 gives 32x21 characters.
    int block_w = 2;
    int block_h = 3;

    int out_w = (width + block_w - 1) / block_w;
    int out_h = (height + block_h - 1) / block_h;

    for (int by = 0; by < out_h; by++) {
        for (int bx = 0; bx < out_w; bx++) {
            unsigned int sum = 0;
            unsigned int count = 0;
            for (int y = by * block_h; y < (by + 1) * block_h && y < height; y++) {
                for (int x = bx * block_w; x < (bx + 1) * block_w && x < width; x++) {
                    sum += data[y * width + x];
                    count++;
                }
            }
            unsigned char avg = (count == 0) ? 0 : (unsigned char)(sum / count);

            int idx = (avg * (ramp_len - 1)) / 255;
            char ch = ramp[idx];
            char s[2] = {ch, '\0'};
            print(s);
        }
        print("\n");
    }
}

// Render bitmap into VGA text mode by mapping average brightness to VGA colors.
const unsigned char vga_palette[16][3] = {
    {0, 0, 0},        // 0: Black
    {0, 0, 170},      // 1: Blue
    {0, 170, 0},      // 2: Green
    {0, 170, 170},    // 3: Cyan
    {170, 0, 0},      // 4: Red
    {170, 0, 170},    // 5: Magenta
    {170, 85, 0},     // 6: Brown (Yellow)
    {170, 170, 170},  // 7: Light Gray
    {85, 85, 85},     // 8: Dark Gray
    {85, 85, 255},    // 9: Bright Blue
    {85, 255, 85},    // 10: Bright Green
    {85, 255, 255},   // 11: Bright Cyan
    {255, 85, 85},    // 12: Bright Red
    {255, 85, 255},   // 13: Bright Magenta
    {255, 255, 85},   // 14: Bright Yellow
    {255, 255, 255}   // 15: White
};


// A function that finds the index of the closest VGA palette colour to a given RGB colour
int find_closest_vga_color(int r, int g, int b) {
    int best_index = 0;
    long best_distance_sq = -1;

    for (int i = 0; i < 16; ++i) {
        int dr = r - vga_palette[i][0];
        int dg = g - vga_palette[i][1];
        int db = b - vga_palette[i][2];
        long distance_sq = dr*dr + dg*dg + db*db;

        if (best_distance_sq == -1 || distance_sq < best_distance_sq) {
            best_distance_sq = distance_sq;
            best_index = i;
        }
    }
    return best_index;
}
void render_bitmap_vga_text(const unsigned char* data, int width, int height) {
    render_bitmap_vga_text_scaled(data, width, height, 80, 25);
}

void render_bitmap_vga_text_scaled(const unsigned char* data, int width, int height, int max_w, int max_h) {
    // Screen dimensions in text mode
    const int screen_w = 80;
    const int screen_h = 25;

    int out_w = max_w;
    int out_h = (height * out_w) / width;
    if (out_h > max_h) {
        out_h = max_h;
        out_w = (width * out_h) / height;
    }
    if (out_w < 1) out_w = 1;
    if (out_h < 1) out_h = 1;

    // Centering offsets
    int offset_x = (screen_w - out_w) / 2;
    if (offset_x < 0) offset_x = 0;
    int offset_y = (screen_h - out_h) / 2;
    if (offset_y < 0) offset_y = 0;

    // For each output cell...
    for (int by = 0; by < out_h; by++) {
        for (int bx = 0; bx < out_w; bx++) {
            int x0 = (bx * width) / out_w;
            int x1 = ((bx + 1) * width) / out_w;
            int y0 = (by * height) / out_h;
            int y1 = ((by + 1) * height) / out_h;

                // *** CHANGE START: Calculate the average RGB colour ***
            unsigned long long sum_r = 0, sum_g = 0, sum_b = 0;
            int count = 0;
            for (int y = y0; y < y1; y++) {
                for (int x = x0; x < x1; x++) {
                    int pixel_index = (y * width + x) * 3;
                    sum_r += data[pixel_index + 0]; // Red
                    sum_g += data[pixel_index + 1]; // Green
                    sum_b += data[pixel_index + 2]; // Blue
                    count++;
                }
            }
            
            unsigned char avg_r = (count == 0) ? 0 : (unsigned char)(sum_r / count);
            unsigned char avg_g = (count == 0) ? 0 : (unsigned char)(sum_g / count);
            unsigned char avg_b = (count == 0) ? 0 : (unsigned char)(sum_b / count);

            // Find the closest VGA palette color to the average RGB color
            int color_index = find_closest_vga_color(avg_r, avg_g, avg_b);
            // *** CHANGE END ***


            // Background color is high nibble; foreground use black (0)
            unsigned char attribute = (unsigned char)((color_index & 0x0F) << 4);
            unsigned short cell = (unsigned short)(' ' & 0x00FF) | ((unsigned short)attribute << 8);

            int sx = offset_x + bx;
            int sy = offset_y + by;
            if (sx < 0 || sx >= screen_w || sy < 0 || sy >= screen_h) continue;

            VIDEO_MEMORY[sy * screen_w + sx] = cell;
        }
    }
}

void render_logo_at_top(
    const unsigned char* data,
    int width,
    int height,
    int max_w,
    int max_h
) {
    const int screen_w = 80;
    const int screen_h = 25;
    const int margin_right = 30; // space on the right

    int out_w = max_w;
    int out_h = (height * out_w) / width;
    if (out_h > max_h) {
        out_h = max_h;
        out_w = (width * out_h) / height;
    }
    if (out_w < 1) out_w = 1;
    if (out_h < 1) out_h = 1;

    // === RIGHT-ALIGNED OFFSET ===
    int offset_x = screen_w - out_w - margin_right;
    if (offset_x < 0) offset_x = 0;

    int offset_y = 0; // top
    for (int by = 0; by < out_h; by++) {
        for (int bx = 0; bx < out_w; bx++) {

            int x0 = (bx * width) / out_w;
            int x1 = ((bx + 1) * width) / out_w;
            int y0 = (by * height) / out_h;
            int y1 = ((by + 1) * height) / out_h;

            unsigned long long sum_r = 0, sum_g = 0, sum_b = 0;
            int count = 0;

            for (int y = y0; y < y1; y++) {
                for (int x = x0; x < x1; x++) {
                    int pixel_index = (y * width + x) * 3;
                    sum_r += data[pixel_index + 0];
                    sum_g += data[pixel_index + 1];
                    sum_b += data[pixel_index + 2];
                    count++;
                }
            }

            unsigned char avg_r = count ? sum_r / count : 0;
            unsigned char avg_g = count ? sum_g / count : 0;
            unsigned char avg_b = count ? sum_b / count : 0;

            int color_index = find_closest_vga_color(avg_r, avg_g, avg_b);

            unsigned char attribute = (color_index & 0x0F) << 4;
            unsigned short cell = ' ' | (attribute << 8);

            int sx = offset_x + bx;
            int sy = offset_y + by;

            if (sx < 0 || sx >= screen_w || sy < 0 || sy >= screen_h)
                continue;

            VIDEO_MEMORY[sy * screen_w + sx] = cell;
        }
    }
}

// for a new keypress (non-zero character) so that accidental buffered
// keys don't make the image disappear immediately.
void wait_for_keypress() {
    // Drain existing scancodes
    while (inb(0x64) & 1) {
        (void)inb(0x60);
    }

    // Now wait for a non-zero character from get_char()
    while (1) {
        char c = get_char();
        if (c != 0) break;
    }
}




// Simple putchar function for compatibility
void putchar(char c) {
    char str[2] = {c, '\0'};
    print(str);
}

// Simple puts function for compatibility
void puts(const char* str) {
    print(str);
}

// String functions
const char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c) {
            return str;
        }
        str++;
    }
    return NULL;
}

// Print unsigned integer function

// Function to convert character to uppercase (ISO-8859-9 specific)
char to_upper_turkish(char c) {
    if (c >= 'a' && c <= 'z') return c - 32; // Basic ASCII
    if (c == '\xF0') return '\xD0'; // ğ -> Ğ
    if (c == '\xFC') return '\xDC'; // ü -> -> Ü
    if (c == '\xB6') return '\xA6'; // ş -> Ş
    if (c == '\xEF') return '\xDD'; // ı -> İ
    if (c == '\xF6') return '\xD6'; // ö -> Ö
    if (c == '\xE7') return '\xC7'; // ç -> Ç
    return c;
}

// Function to convert character to lowercase (ISO-8859-9 specific)
char to_lower_turkish(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32; // Basic ASCII
    if (c == '\xD0') return '\xF0'; // Ğ -> ğ
    if (c == '\xDC') return '\xFC'; // Ü -> ü
    if (c == '\xA6') return '\xB6'; // Ş -> ş
    if (c == '\xDD') return '\xEF'; // İ -> ı
    if (c == '\xD6') return '\xF6'; // Ö -> ö
    if (c == '\xC7') return '\xE7'; // Ç -> ç
    return c;
}

char get_char() {
    unsigned char scancode;
    while (1) {
        if ((inb(0x64) & 1) == 0) {
            continue;
        }
        scancode = inb(0x60); // Read scancode from data port (0x60)

        // Handle key release (break codes have MSB set)
        if (scancode & 0x80) {
            // Key release event
            scancode &= 0x7F; // Clear MSB to get make code

            // Handle Shift key release
            if (scancode == 0x2A || scancode == 0x36) { // Left Shift (0x2A) or Right Shift (0x36)
                shift_active = false;
            }
            // Handle Ctrl key release
            else if (scancode == 0x1D) { // Left Ctrl
                ctrl_active = false;
            }
            return 0; // Return null char for non-character keys on release
        } else {
            // Key press event (make code)

            // Handle Shift key press
            if (scancode == 0x2A || scancode == 0x36) { // Left Shift (0x2A) or Right Shift (0x36)
                shift_active = true;
                return 0; // Don't produce a character for Shift key itself
            }
            // Handle Ctrl key press
            else if (scancode == 0x1D) { // Left Ctrl
                ctrl_active = true;
                return 0; // Don't produce a character for Ctrl key itself
            }
            // Handle Caps Lock toggle
            else if (scancode == 0x3A) { // Caps Lock
                caps_lock_active = !caps_lock_active;
                return 0; // Don't produce a character for Caps Lock key itself
            }

            // Handle Ctrl key combinations
            if (ctrl_active) {
                switch (scancode) {
                    case 0x1F: // Ctrl+S
                        print("\nCtrl+S detected!\n"); // Debug print
                        return 0x13;
                    case 0x2D: return 0x18; // Ctrl+X
                    case 0x11: return 0x11; // Ctrl+Q
                    default: return 0; // Other Ctrl combinations
                }
            }

            // Handle arrow keys
            if (scancode == 0x48) { // Up arrow
                return 0x01; // Special code for up arrow
            }
            if (scancode == 0x50) { // Down arrow
                return 0x02; // Special code for down arrow
            }
            if (scancode == 0x4B) { // Left arrow
                return 0x03; // Special code for left arrow
            }
            if (scancode == 0x4D) { // Right arrow
                return 0x04; // Special code for right arrow
            }

            // Get character from appropriate keymap
            if (shift_active) {
                char key_char = tr_q_keymap_shifted[scancode];

                // Apply Caps Lock logic for alphabetic characters (including Turkish ones)
                if (caps_lock_active) {
                    // Check if the character is an alphabet letter (ASCII or Turkish)
                    bool is_alpha = (key_char >= 'a' && key_char <= 'z') ||
                    (key_char >= 'A' && key_char <= 'Z') ||
                    (key_char == '\xF0' || key_char == '\xD0') || // ğĞ
                    (key_char == '\xFC' || key_char == '\xDC') || // üÜ
                    (key_char == '\xB6' || key_char == '\xA6') || // şŞ
                    (key_char == '\xEF' || key_char == '\xDD') || // ıİ
                    (key_char == '\xF6' || key_char == '\xD6') || // öÖ
                    (key_char == '\xE7' || key_char == '\xC7');   // çÇ

                    if (is_alpha) {
                        if (shift_active) {
                            // If Shift is active, Caps Lock reverses the case
                            key_char = to_lower_turkish(key_char);
                        } else {
                            // If Shift is not active, Caps Lock makes it uppercase
                            key_char = to_upper_turkish(key_char);
                        }
                    }
                }

                return key_char;
            } else {
                char key_char = tr_q_keymap[scancode];

                // Apply Caps Lock logic for alphabetic characters (including Turkish ones)
                if (caps_lock_active) {
                    // Check if the character is an alphabet letter (ASCII or Turkish)
                    bool is_alpha = (key_char >= 'a' && key_char <= 'z') ||
                    (key_char >= 'A' && key_char <= 'Z') ||
                    (key_char == '\xF0' || key_char == '\xD0') || // ğĞ
                    (key_char == '\xFC' || key_char == '\xDC') || // üÜ
                    (key_char == '\xB6' || key_char == '\xA6') || // şŞ
                    (key_char == '\xEF' || key_char == '\xDD') || // ıİ
                    (key_char == '\xF6' || key_char == '\xD6') || // öÖ
                    (key_char == '\xE7' || key_char == '\xC7');   // çÇ

                    if (is_alpha) {
                        if (shift_active) {
                            // If Shift is active, Caps Lock reverses the case
                            key_char = to_lower_turkish(key_char);
                        } else {
                            // If Shift is not active, Caps Lock makes it uppercase
                            key_char = to_upper_turkish(key_char);
                        }
                    }
                }

                return key_char;
            }
        }
    }
}

void gets(char* buf) {
    int i = 0;
    while (1) {
        char c = get_char();

        if (c == '\n') { // Enter key
            print("\n");
            break;
        }

        if (c == '\b') { // Backspace
            if (i > 0) {
                i--;
                print("\b"); // print() now handles the visual backspace
            }
            continue; // Don't add backspace to buffer
        }

        // Only add printable characters to the buffer
        // (c != 0) check also handles cases where modifier keys produce 0
        if (c != 0) {
            buf[i++] = c;
            char s[2] = {c, '\0'};
            print(s); // Echo character to screen
        }

        // Prevent buffer overflow
        if (i >= 127) { // Max buffer size (128) - 1 for null terminator
            break;
        }
    }
    buf[i] = '\0';
}

void wait(unsigned int seconds) {
    for (volatile unsigned long i = 0; i < seconds * 100000000UL; i++) {
        // Busy wait, yaklaşık saniye başına 100M iterasyon
    }
}

// ----------------- Terminal ---------------------------

void run_terminal() {
    char command[20];
    char arg[80];

    Folder* root = allocate_folder();
    strcpy(root->name, "YoskHome");
    root->parent = NULL;
    root->child_count = 0;

    Folder* current = root;
    char input[128];

    print_ascii_art("\n");
    print_ascii_art("\nyoskOS\n\n");
    print_color("========================================\n", 0x0E);
    print_color("          Welcome to yoskOS\n", 0x0A);
    print_color("========================================\n", 0x0E);
    print_color("A simple operating system built with C\n", 0x07);
    print_color("Type 'help' for available commands.\n\n", 0x07);

    while (1) {
        char path[128];
        get_path_string(current, path, sizeof(path)); 
        draw_toolbar(username, path);
        print_color("yoskOS", 0x0A);
        print_color(":", 0x07);
        print_color(path, 0x0B);
        print_color("$ ", 0x07);

        get_line(input, sizeof(input));
        parse_input(input, command, arg);

        if (input[0] != '\0') 
        { 
            strncat(log_buffer, input, sizeof(log_buffer) - strlen(log_buffer) - 1);
            strncat(log_buffer, "\n", sizeof(log_buffer) - strlen(log_buffer) - 1);
        } 
        
        if (input[0] == '\0') continue;

          if (strcmp(command, "show") == 0) {
            print("Available images: 1-2-3-4");
            print_int(num_available_images);
            print("\nEnter the image number to display: \n");

            char choice_buffer[128];
            gets(choice_buffer);     // Call your function to get user input
            
            int image_num = simple_atoi(choice_buffer);

            if (image_num > 0 && image_num <= num_available_images) {
                const Image selected_image = available_images[image_num - 1];

                clear_screen();
                render_bitmap_vga_text(selected_image.data, selected_image.width, selected_image.height);
                
                cursor = 80 * 24;
                print("The visual display is complete. Press a button to continue....\n");
                
                // The `void` version of wait_for_keypress here
                // Only waits for a key to be pressed, it does not capture the character.
                wait_for_keypress(); 
                
                clear_screen();
                print_ascii_art("\nyoskOS\n\n");
                print("Type “clear” to clear\n");
            } else {
                print("Error: Invalid or unavailable number.\n");
            }
        }
        else if (strncmp(input, "ascii ", 6) == 0) { // strncmp is now defined and linked
            print("\n");
            print_ascii_art(input + 6);
        }
        else if (strcmp(command, "screen") == 0) {
            print_color("Enter Screen Size\n", 0x0F);

            print_color("Width: ", 0x07);
            print("\n");
            char width_buffer[16];
            gets(width_buffer);
            int width = simple_atoi(width_buffer);

            print_color("Height: ", 0x07);
            print("\n");
            char height_buffer[16];
            gets(height_buffer);
            int height = simple_atoi(height_buffer);

            if (width > 0 && height > 0 && width <= 80 && height <= 24) {
                screen_width = width;
                screen_height = height;
                print_color("Screen size updated.\n", 0x0A);
            } else {
                print_color("Error: Invalid screen size.\n", 0x0C);
            }
        }

        
        else if (strcmp(command, "mkdir") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: mkdir <name>\n");
                continue;
            }
            if (create_folder(arg, current)) {
                print("Folder created: ");
                print(arg);
                print("\n");
            } else {
                print("Folder could not be created (already exists or no space?)\n");
            }

        }
        else if (strcmp(command, "not") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: not <filename>\n");
                continue;
            }
            char *args[] = {"not", arg};  // Create argument array
            not_start(2, args);  // Call with 2 arguments (program name and file name)
        }
         else if (strcmp(command, "ls") == 0) {
            fs_list();
        }
        else if (strcmp(command, "cd") == 0) {
            if (strcmp(arg, "..") == 0 && current->parent != NULL) {
                current = current->parent;
            } else if (strlen(arg) == 0 || strcmp(arg, "~") == 0) { // cd without arg or cd ~ goes to root
                current = root;
            }
            else {
                Folder* next = find_subfolder(current, arg);
                if (next) current = next;
                else {
                    print("Folder not found: ");
                    print(arg);
                    print("\n");
                }
            }

        }
        else if (strncmp(input, "write ", 6) == 0) { // strncmp is now defined and linked
            // Find the first space to split filename and content
            char* name_start = input + 6;
            char* content_start = name_start;
            while (*content_start && *content_start != ' ') {
                content_start++;
            }

            if (*content_start == ' ') {
                *content_start = '\0'; // Null-terminate the filename
                content_start++; // Move past the null terminator to content start

                // Trim leading spaces from content
                while (*content_start == ' ') {
                    content_start++;
                }

                if (strlen(name_start) == 0) {
                    print("Usage: write <filename> <content>\n");
                } else if (fs_create(name_start, content_start) == 0) {
                    print("File written.\n");
                } else {
                    print("Failed to write file (no space?)\n");
                }
            } else {
                print("Usage: write <filename> <content>\n");
            }
        }
        else if (strcmp(command, "cat") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: cat <filename>\n");
                continue;
            }
            const char* content = fs_read(arg);
            if (content) {
                print(content);
                print("\n");
            } else {
                print("File not found.\n");
            }
        }
        else if (strcmp(command, "pwd") == 0) {
            print_path(current);
            print("\n");

        }
        else if (strcmp(command, "exit") == 0) {
            print_color("System shutting down in 5 seconds...\n", 0x04);
            wait(1);
            print_color("System shutting down in 4 seconds...\n", 0x04);
            wait(1);
            print_color("System shutting down in 3 seconds...\n", 0x04);
            wait(1);
            print_color("System shutting down in 2 seconds...\n", 0x04);
            wait(1);
            print_color("System shutting down in 1 second...\n", 0x04);
            wait(1);
            print_color("System shutdown complete.\n", 0x04);
            qemu_exit();
            kernel_exit();
        }

        else if (strcmp(command, "ascii") == 0) {
            print("\n");
            print_ascii_art(arg);
        }
        else if (strcmp(command, "help") == 0) {
            clear_screen();
            cursor = screen_width * 0;
            print_color("========================================\n", 0x0E);
            print_color("          yoskOS Help Menu\n", 0x0E);
            print_color("========================================\n\n", 0x0E);
            
            print_color("File System Commands:\n", 0x0B);
            print_color("  ls     -> Lists directory contents\n", 0x07);
            print_color("  cd     -> Changes directory (cd <name> or cd ..)\n", 0x07);
            print_color("  pwd    -> Shows the current path\n", 0x07);
            print_color("  mkdir  -> Creates a directory (mkdir <name>)\n", 0x07);
            print_color("  write  -> Creates a file (write <filename> <content>)\n", 0x07);
            print_color("  cat    -> Displays file contents (cat <filename>)\n", 0x07);
            print("\n");
            print_color("Press space for more...\n", 0x08);
            while (1) {
                char c = get_char();
                if (c == ' ') break;
            }
            
            print_color("Applications:\n", 0x0C);
            print_color("  not    -> Opens the Notepad application\n", 0x07);
            print_color("  game   -> Opens the game menu\n", 0x07);
            print_color("  show   -> Image viewer (show)\n", 0x07);
            print_color("  ascii  -> Displays ASCII art (ascii <text>)\n", 0x07);
            print("\n");
            print_color("Press space for more...\n", 0x08);
            while (1) {
                char c = get_char();
                if (c == ' ') break;
            }
            
            print_color("System Commands:\n", 0x0D);
            print_color("  info   -> System information (info, info -a, etc.)\n", 0x07);
            print_color("  screen -> Adjusts screen resolution\n", 0x07);
            print_color("  colorbg-> Changes background color (colorbg <color>)\n", 0x07);
            print_color("          Available colors: black, blue, green, cyan, red, magenta, brown,\n", 0x07);
            print_color("          lightgray, darkgray, lightblue, lightgreen, lightcyan, lightred,\n", 0x07);
            print_color("          lightmagenta, yellow, white\n", 0x07);
            print_color("  log    -> Saves command history to file\n", 0x07);
            print_color("  clear  -> Clears the screen\n", 0x07);
            print_color("  help   -> Shows this menu\n", 0x07);
            print_color("  exit   -> Shuts down the system\n", 0x07);
            print("\n");
            print_color("Type any command to continue...\n", 0x08);


        } else if (strcmp(command, "log") == 0) {
            if (fs_create("inputs.txt", log_buffer) == 0) {
                print("The commands have been saved to the inputs.txt file..\n");
            } else {
                print("Failed to create file.\n");
            }

        }
        else if (strcmp(command, "info") == 0) {
            show_info(arg);
        }
        else if (strcmp(command, "clear") == 0) {
            clear_screen();

        } else if (strcmp(command, "colorbg") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: colorbg <color>\nAvailable colors: black, blue, green, cyan, red, magenta, brown, lightgray, darkgray, lightblue, lightgreen, lightcyan, lightred, lightmagenta, yellow, white\n");
            } else {
                int color_code = get_color_code(arg);
                if (color_code != -1) {
                    bg_color = color_code;
                    print("Background color changed to ");
                    print(arg);
                    print(".\n");
                } else {
                    print("Invalid color: ");
                    print(arg);
                    print("\n");
                }
            }

        } else if (strcmp(command, "game") == 0) {
            clear_screen();
            print("Games:\n");
            print("1. Tic Tac Toe\n");
            print("2. Hangman\n");
            print("Make a choice: \n");
            char game_input[128];
            get_line(game_input, sizeof(game_input));
            int choice = simple_atoi(game_input);

            if (choice == 1) {
                play_xox();
            } else if (choice == 2) {
                play_hangman();
            } else {
                print("Invalid choice\n");
            }

            print("Press any key to exit the game...\n");
            wait_for_keypress();
            clear_screen();
        } else {
            print_color("Unknown command: ", 0x04);
            print(command);
            print("\n");
        }
    }
}


// ----------------- Kernel Entry -------------------------

void kernel_main() {
    print("kernel active");
    set_vga_mode(TEXT_MODE);
    clear_screen();
    print_color("Enter your username: ",0x0A);
    gets(username);
    clear_screen();
    run_terminal();
}
