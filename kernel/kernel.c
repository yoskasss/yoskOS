#include <stdbool.h> 
#include <stddef.h> 
#include <stdint.h>
#include "../include/string.h"  
#include "../include/notepad.h" 
#include "../include/uptime.h"  
#include "../include/images.h"  
#include "../include/console.h"  
#include "../include/info.h"
#include "../include/process.h"  

volatile unsigned int timer_ticks = 0;

char log_buffer[4096] = "";

int screen_width = 80;
int screen_height = 24;
int bg_color = 0;

char* strcpy(char* dest, const char* src);
int not_start(int argc, char *argv[], int mode);
void gets(char* buf);
void mini_terminal();
void render_bitmap_vga_text_scaled(const unsigned char* data, int width, int height, int max_w, int max_h);
void play_xox();
void play_hangman();
void notepad_entry();
void xox_entry();
void hangman_entry();

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
    {
        "EEEEE",
        "E    ",
        "EEE  ",
        "E    ",
        "EEEEE"
    },
    {
        "FFFFF",
        "F    ",
        "FFF  ",
        "F    ",
        "F    "
    },
    {
        "GGGGG",
        "G    ",
        "G GGG",
        "G   G",
        " GGG "
    },
    {
        "H   H",
        "H   H",
        "HHHHH",
        "H   H",
        "H   H"
    },
    {
        "IIIII",
        "  I  ",
        "  I  ",
        "  I  ",
        "IIIII"
    },
    {
        "JJJJJ",
        "    J",
        "    J",
        "J   J",
        " JJJ "
    },
    {
        "K   K",
        "K  K ",
        "KKK  ",
        "K  K ",
        "K   K"
    },
    {
        "L    ",
        "L    ",
        "L    ",
        "L    ",
        "LLLLL"
    },
    {
        "M   M",
        "MM MM",
        "M M M",
        "M   M",
        "M   M"
    },
    {
        "N   N",
        "NN  N",
        "N N N",
        "N  NN",
        "N   N"
    },
    {
        " OOO ",
        "O   O",
        "O   O",
        "O   O",
        " OOO "
    },
    {
        "PPPP ",
        "P   P",
        "PPPP ",
        "P    ",
        "P    "
    },
    {
        " QQQ ",
        "Q   Q",
        "Q Q Q",
        "Q  Q ",
        " QQ Q"
    },
    {
        "RRRR ",
        "R   R",
        "RRRR ",
        "R  R ",
        "R   R"
    },
    {
        " SSSS",
        "S    ",
        " SSS ",
        "    S",
        "SSSS "
    },
    {
        "TTTTT",
        "  T  ",
        "  T  ",
        "  T  ",
        "  T  "
    },
    {
        "U   U",
        "U   U",
        "U   U",
        "U   U",
        " UUU "
    },
    {
        "V   V",
        "V   V",
        "V   V",
        " V V ",
        "  V  "
    },
    {
        "W   W",
        "W   W",
        "W W W",
        "WW WW",
        "W   W"
    },
    {
        "X   X",
        " X X ",
        "  X  ",
        " X X ",
        "X   X"
    },
    {
        "Y   Y",
        " Y Y ",
        "  Y  ",
        "  Y  ",
        "  Y  "
    },
    {
        "ZZZZZ",
        "   Z ",
        "  Z  ",
        " Z   ",
        "ZZZZZ"
    }
};

char tr_q_keymap[128] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '-', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\xF0', '\xFC', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\xB6', '\xEF', ',', 0,   '<', 'z',
    'x', 'c', 'v', 'b', 'n', 'm', '\xF6', '\xE7', '.', 0,   '*', 0,   ' ',
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '7', '8',
    '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0
};

char tr_q_keymap_shifted[128] = {
    0,    27,  '!', '\'', '^', '+', '%', '&', '/', '(', ')', '=', '?', '_', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '\xD0', '\xDC', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\xA6', '\xDD', ';', 0,   '>', 'Z',
    'X', 'C', 'V', 'B', 'N', 'M', '\xD6', '\xC7', ':', 0,   '*', 0,   ' ',
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '7', '8',
    '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0
};

#define VGA_TEXT_WIDTH  80
#define VGA_TEXT_HEIGHT 25
#define VGA_ADDR   0xB8000

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
    for (int i = 0; i < 1920; i++) {
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
    for (int i = 0; i < 80; i++) {
        history_line[i] = VIDEO_MEMORY[i] & 0xFF;
    }
    history_line[80] = '\0';

    for (int i = 1999; i >= 80; i--) {
        VIDEO_MEMORY[i] = VIDEO_MEMORY[i - 80];
    }
    for (int i = 0; i < 80; i++) {
        VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
    }
    if (cursor < 80) {
        cursor = 80;
    }
}

void draw_toolbar(const char* user, const char* path) {
    clear_line(0);

    vga_print_at("User: ", 0, 0, 0x0D);
    vga_print_at(user, 6, 0, 0x0D);
    int path_start = 6 + strlen(user) + 2;
    vga_print_at("Path: ", path_start, 0, 0x0D);
    vga_print_at(path, path_start + 6, 0, 0x0D);

    double uptime = get_uptime_seconds(1000000000);
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

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

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
    asm volatile ("cli");
    while (1) {
        asm volatile ("hlt");
    }
}

void render_bitmap_ascii(const unsigned char* data, int width, int height) {

    const char* ramp = " @%#*+=-:. ";
    const int ramp_len = 10;

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

const unsigned char vga_palette[16][3] = {
    {0, 0, 0},
    {0, 0, 170},
    {0, 170, 0},
    {0, 170, 170},
    {170, 0, 0},
    {170, 0, 170},
    {170, 85, 0},
    {170, 170, 170},
    {85, 85, 85},
    {85, 85, 255},
    {85, 255, 85},
    {85, 255, 255},
    {255, 85, 85},
    {255, 85, 255},
    {255, 255, 85},
    {255, 255, 255}
};

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

    int offset_x = (screen_w - out_w) / 2;
    if (offset_x < 0) offset_x = 0;
    int offset_y = (screen_h - out_h) / 2;
    if (offset_y < 0) offset_y = 0;

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
            
            unsigned char avg_r = (count == 0) ? 0 : (unsigned char)(sum_r / count);
            unsigned char avg_g = (count == 0) ? 0 : (unsigned char)(sum_g / count);
            unsigned char avg_b = (count == 0) ? 0 : (unsigned char)(sum_b / count);

            int color_index = find_closest_vga_color(avg_r, avg_g, avg_b);

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
    const int margin_right = 30;

    int out_w = max_w;
    int out_h = (height * out_w) / width;
    if (out_h > max_h) {
        out_h = max_h;
        out_w = (width * out_h) / height;
    }
    if (out_w < 1) out_w = 1;
    if (out_h < 1) out_h = 1;

    int offset_x = screen_w - out_w - margin_right;
    if (offset_x < 0) offset_x = 0;

    int offset_y = 0;
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

void wait_for_keypress() {

    while (inb(0x64) & 1) {
        (void)inb(0x60);
    }

    while (1) {
        char c = get_char();
        if (c == 0x14) {
            mini_terminal();
            continue;
        }
        if (c != 0) break;
    }
}

void putchar(char c) {
    char str[2] = {c, '\0'};
    print(str);
}

void puts(const char* str) {
    print(str);
}

const char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c) {
            return str;
        }
        str++;
    }
    return NULL;
}

char to_upper_turkish(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    if (c == '\xF0') return '\xD0';
    if (c == '\xFC') return '\xDC';
    if (c == '\xB6') return '\xA6';
    if (c == '\xEF') return '\xDD';
    if (c == '\xF6') return '\xD6';
    if (c == '\xE7') return '\xC7';
    return c;
}

char to_lower_turkish(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    if (c == '\xD0') return '\xF0';
    if (c == '\xDC') return '\xFC';
    if (c == '\xA6') return '\xB6';
    if (c == '\xDD') return '\xEF';
    if (c == '\xD6') return '\xF6';
    if (c == '\xC7') return '\xE7';
    return c;
}

char get_char() {
    unsigned char scancode;
    
    if ((inb(0x64) & 1) == 0) {
        return 0;
    }
    
    scancode = inb(0x60);

    if (scancode & 0x80) {
        scancode &= 0x7F;

        if (scancode == 0x2A || scancode == 0x36) {
            shift_active = false;
        } else if (scancode == 0x1D) {
            ctrl_active = false;
        }
        return 0;
    }
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_active = true;
        return 0;
    }

    if (scancode == 0x1D) {
        ctrl_active = true;
        return 0;
    }

    if (scancode == 0x3A) {
        caps_lock_active = !caps_lock_active;
        return 0;
    }

    if (ctrl_active) {
        switch (scancode) {
            case 0x1F:
                print("\nCtrl+S detected!\n");
                return 0x13;
            case 0x14: return 0x14;
            case 0x2D: return 0x18;
            case 0x11: return 0x11;
            default: return 0;
        }
    }

    if (scancode == 0x48) {
        return 0x01;
    }
    if (scancode == 0x50) {
        return 0x02;
    }
    if (scancode == 0x4B) {
        return 0x03;
    }
    if (scancode == 0x4D) {
        return 0x04;
    }

    if (shift_active) {
        char key_char = tr_q_keymap_shifted[scancode];

        if (caps_lock_active) {
            bool is_alpha = (key_char >= 'a' && key_char <= 'z') ||
            (key_char >= 'A' && key_char <= 'Z') ||
            (key_char == '\xF0' || key_char == '\xD0') ||
            (key_char == '\xFC' || key_char == '\xDC') ||
            (key_char == '\xB6' || key_char == '\xA6') ||
            (key_char == '\xEF' || key_char == '\xDD') ||
            (key_char == '\xF6' || key_char == '\xD6') ||
            (key_char == '\xE7' || key_char == '\xC7');

            if (is_alpha) {
                if (shift_active) {
                    key_char = to_lower_turkish(key_char);
                } else {
                    key_char = to_upper_turkish(key_char);
                }
            }
        }

        return key_char;
    } else {
        char key_char = tr_q_keymap[scancode];

        if (caps_lock_active) {
            bool is_alpha = (key_char >= 'a' && key_char <= 'z') ||
            (key_char >= 'A' && key_char <= 'Z') ||
            (key_char == '\xF0' || key_char == '\xD0') ||
            (key_char == '\xFC' || key_char == '\xDC') ||
            (key_char == '\xB6' || key_char == '\xA6') ||
            (key_char == '\xEF' || key_char == '\xDD') ||
            (key_char == '\xF6' || key_char == '\xD6') ||
            (key_char == '\xE7' || key_char == '\xC7');

            if (is_alpha) {
                if (shift_active) {
                    key_char = to_lower_turkish(key_char);
                } else {
                    key_char = to_upper_turkish(key_char);
                }
            }
        }

        return key_char;
    }
}

void gets(char* buf) {
    int i = 0;
    while (1) {
        char c = get_char();
        
        if (c == 0) {
            process_yield();
            continue;
        }

        if (c == '\n') {
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
            buf[i++] = c;
            char s[2] = {c, '\0'};
            print(s);
        }

        if (i >= 127) {
            break;
        }
    }
    buf[i] = '\0';
}

void wait(unsigned int seconds) {
    for (volatile unsigned long i = 0; i < seconds * 100000000UL; i++) {

    }
}

void draw_gui_menu() {

    const char* menu_items[] = {
        "Console",
        "Notepad",
        "Games",
        "Task Manager"
    };
    
    int num_items = 4;
    int selected = 0;
    int items_per_row = 2;
    int need_redraw = 1;
    
    while (1) {
        if (need_redraw) {
            clear_screen();
            
            print_color("======================================================\n", 0x0F);
            print_color("           yoskOS GUI Menu System\n", 0x0F);
            print_color("======================================================\n", 0x07);
            print("\n\n");
            

            for (int col = 0; col < 2; col++) {
                int idx = col;
                if (idx >= num_items) break;
                
                unsigned char color = (idx == selected) ? 0x0E : 0x07;
                
                print_color("  ", 0x07);
                print_color("+---------------------+", color);
                print_color("  ", 0x07);
            }
            print("\n");
            
            for (int line = 0; line < 6; line++) {
                for (int col = 0; col < 2; col++) {
                    int idx = col;
                    if (idx >= num_items) break;
                    
                    unsigned char color = (idx == selected) ? 0x0E : 0x07;
                    
                    print_color("  ", 0x07);
                    print_color("|", color);
                    
                    if (line == 0) {

                        char num_str[2];
                        num_str[0] = '1' + idx;
                        num_str[1] = '\0';
                        print_color(" ", color);
                        print_color(num_str, 0x0C);
                        for (int p = 0; p < 16; p++) print_color(" ", color);
                    } else if (line == 2) {

                        const char* text = menu_items[idx];
                        int text_len = strlen(text);
                        int padding = (19 - text_len) / 2;
                        
                        for (int p = 0; p < padding; p++) print_color(" ", color);
                        print_color(text, color);
                        for (int p = 0; p < 19 - padding - text_len; p++) print_color(" ", color);
                    } else {
                        print_color("                   ", color);
                    }
                    
                    print_color("|", color);
                    print_color("  ", 0x07);
                }
                print("\n");
            }
            
            for (int col = 0; col < 2; col++) {
                int idx = col;
                if (idx >= num_items) break;
                
                unsigned char color = (idx == selected) ? 0x0E : 0x07;
                
                print_color("  ", 0x07);
                print_color("+---------------------+", color);
                print_color("  ", 0x07);
            }
            print("\n\n");
            

            for (int col = 0; col < 2; col++) {
                int idx = 2 + col;
                if (idx >= num_items) break;
                
                unsigned char color = (idx == selected) ? 0x0E : 0x07;
                
                print_color("  ", 0x07);
                print_color("+---------------------+", color);
                print_color("  ", 0x07);
            }
            print("\n");
            
            for (int line = 0; line < 6; line++) {
                for (int col = 0; col < 2; col++) {
                    int idx = 2 + col;
                    if (idx >= num_items) break;
                    
                    unsigned char color = (idx == selected) ? 0x0E : 0x07;
                    
                    print_color("  ", 0x07);
                    print_color("|", color);
                    
                    if (line == 0) {

                        char num_str[2];
                        num_str[0] = '3' + col;
                        num_str[1] = '\0';
                        print_color(" ", color);
                        print_color(num_str, 0x0C);
                        for (int p = 0; p < 16; p++) print_color(" ", color);
                    } else if (line == 2) {

                        const char* text = menu_items[idx];
                        int text_len = strlen(text);
                        int padding = (19 - text_len) / 2;
                        
                        for (int p = 0; p < padding; p++) print_color(" ", color);
                        print_color(text, color);
                        for (int p = 0; p < 19 - padding - text_len; p++) print_color(" ", color);
                    } else {
                        print_color("                   ", color);
                    }
                    
                    print_color("|", color);
                    print_color("  ", 0x07);
                }
                print("\n");
            }
            
            for (int col = 0; col < 2; col++) {
                int idx = 2 + col;
                if (idx >= num_items) break;
                
                unsigned char color = (idx == selected) ? 0x0E : 0x07;
                
                print_color("  ", 0x07);
                print_color("+---------------------+", color);
                print_color("  ", 0x07);
            }
            print("\n\n");
            
            print_color("Use Arrow Keys/1234 to Navigate | ENTER to Select | ESC to Exit\n", 0x0A);
            need_redraw = 0;
        }
        
        char c = get_char();
        
        if (c == 0) continue;
        
        if (c == 0x14) {
            mini_terminal();
            need_redraw = 1;
            continue;
        }

        if (c == '1') {
            selected = 0;
            need_redraw = 1;
        }
        else if (c == '2') {
            selected = 1;
            need_redraw = 1;
        }
        else if (c == '3') {
            selected = 2;
            need_redraw = 1;
        }
        else if (c == '4') {
            selected = 3;
            need_redraw = 1;
        }

        else if (c == 0x01) {
            selected = (selected - items_per_row + num_items) % num_items;
            need_redraw = 1;
        }
        else if (c == 0x02) {
            selected = (selected + items_per_row) % num_items;
            need_redraw = 1;
        }
        else if (c == 0x03) {
            if (selected % items_per_row > 0) {
                selected--;
                need_redraw = 1;
            }
        }
        else if (c == 0x04) {
            if (selected % items_per_row < items_per_row - 1 && selected < num_items - 1) {
                selected++;
                need_redraw = 1;
            }
        }
        else if (c == '\n') {
            clear_screen();
            
            if (selected == 0) {

                break;
            }
            else if (selected == 1) {

                print_color("Opening Notepad...\n\n", 0x0A);
                char *argv[] = {"not", "task.txt"};
                not_start(2, argv, 0);
            }
            else if (selected == 2) {

                print_color("======================================================\n", 0x0F);
                print_color("                  Games\n", 0x0F);
                print_color("======================================================\n", 0x07);
                print("\n1. Tic Tac Toe\n");
                print("2. Hangman\n\n");
                print_color("Make a choice: ", 0x0A);
                char game_input[128];
                get_line(game_input, sizeof(game_input));
                int choice = simple_atoi(game_input);
                if (choice == 1) {
                    play_xox();
                } else if (choice == 2) {
                    play_hangman();
                } else {
                    print_color("Invalid choice\n", 0x04);
                }
                print("\nPress any key to return to menu...\n");
                wait_for_keypress();
            }
            else if (selected == 3) {

                print_color("======================================================\n", 0x0F);
                print_color("            Process Task Manager\n", 0x0F);
                print_color("======================================================\n", 0x07);
                print("\n");
                process_list();
                print("\n");
                print_color("Press any key to return to menu...\n", 0x0A);
                wait_for_keypress();
            }
        }
        else if (c == 27) {
            break;
        }
    }
}

void mini_terminal() {
    clear_screen();
    
    print_color("=====================================\n", 0x0F);
    print_color("         Mini Terminal - Task Manager\n", 0x0F);
    print_color("=====================================\n\n", 0x0F);
    print_color("Type 'quit' or 'exit' to close | 'ps' to list processes\n", 0x0A);
    print_color("Use 'pskill <pid>' to terminate process\n\n", 0x0A);
    
    process_list();
    
    while (1) {
        print_color("\nMini Terminal> ", 0x0B);
        
        char cmd[128];
        get_line(cmd, sizeof(cmd));
        
        if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        }
        else if (strcmp(cmd, "ps") == 0) {
            process_list();
        }
        else if (strncmp(cmd, "pskill ", 7) == 0) {
            int pid = simple_atoi(cmd + 7);
            int result = process_kill(pid);
            if (result == -1) {
                print_color("Error: Process not found\n", 0x04);
            } else if (result == -2) {
                print_color("Error: Cannot kill system process\n", 0x04);
            }
        }
        else if (cmd[0] != '\0') {
            print_color("Unknown command: '", 0x0E);
            print_color(cmd, 0x0E);
            print_color("'\n", 0x0E);
        }
    }
    
    clear_screen();
}

void shell_entry() {

    static char command[20];
    static char arg[80];
    static Folder* root = NULL;
    static Folder* current = NULL;
    static char input[128];
    static int initialized = 0;
    

    if (!initialized) {
        root = allocate_folder();
        strcpy(root->name, "YoskHome");
        root->parent = NULL;
        root->child_count = 0;
        current = root;

        print_color("========================================\n", 0x0E);
        print_color("          Shell Process Started\n", 0x0A);
        print_color("========================================\n", 0x0E);
        print_color("Type 'help' for available commands.\n", 0x07);
        print_color("Type 'gui' to open GUI Menu System.\n\n", 0x0A);
        initialized = 1;
    }

    while (1) {
        char path[128];
        get_path_string(current, path, sizeof(path));
        draw_toolbar(username, path);
        print_color("yoskOS", 0x0A);
        print_color(":", 0x07);
        print_color(path, 0x0B);
        print_color("$ ", 0x07);
        
        int input_idx = 0;
        while (1) {
            char c = get_char();
            
            if (c == 0x14) {
                print("\n");
                mini_terminal();
                print_color("yoskOS", 0x0A);
                print_color(":", 0x07);
                print_color(path, 0x0B);
                print_color("$ ", 0x07);
                input_idx = 0;
                continue;
            }
            
            if (c == '\n') {
                print("\n");
                break;
            }
            
            if (c == '\b') {
                if (input_idx > 0) {
                    input_idx--;
                    print("\b");
                }
                continue;
            }
            
            if (c != 0 && input_idx < 127) {
                input[input_idx++] = c;
                char s[2] = {c, '\0'};
                print(s);
            }
        }
        input[input_idx] = '\0';
        parse_input(input, command, arg);
        
        if (input[0] != '\0') {
            strncat(log_buffer, input, sizeof(log_buffer) - strlen(log_buffer) - 1);
            strncat(log_buffer, "\n", sizeof(log_buffer) - strlen(log_buffer) - 1);
        }
        if (input[0] == '\0') continue;

        if (strcmp(command, "show") == 0) {
            print("Available images: 1-2-3-4");
            print_int(num_available_images);
            print("\nEnter the image number to display: \n");

            char choice_buffer[128];
            gets(choice_buffer);
            
            int image_num = simple_atoi(choice_buffer);

            if (image_num > 0 && image_num <= num_available_images) {
                const Image selected_image = available_images[image_num - 1];

                clear_screen();
                render_bitmap_vga_text(selected_image.data, selected_image.width, selected_image.height);
                
                cursor = 80 * 24;
                print("The visual display is complete. Press a button to continue....\n");
                
                wait_for_keypress(); 
                
                clear_screen();
                print_ascii_art("\nyoskOS\n\n");
                print("Type 'clear' to clear\n");
            } else {
                print("Error: Invalid or unavailable number.\n");
            }
        }
        else if (strncmp(input, "ascii ", 6) == 0) {
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
        else if (strcmp(command, "write") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: write <filename> <content>\n");
            } else {
                char* content_start = input;
                while (*content_start && *content_start != ' ') {
                    content_start++;
                }
                content_start++;
                while (*content_start && *content_start != ' ') {
                    content_start++;
                }
                if (*content_start == ' ') {
                    content_start++;
                    if (fs_create(arg, content_start) == 0) {
                        print("File written.\n");
                    } else {
                        print("Failed to write file (no space?)\n");
                    }
                } else {
                    print("Usage: write <filename> <content>\n");
                }
            }
        }
        else if (strcmp(command, "cat") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: cat <filename>\n");
            } else {
                const char* content = fs_read(arg);
                if (content) {
                    print(content);
                    print("\n");
                } else {
                    print("File not found.\n");
                }
            }
        }
        else if (strcmp(command, "not") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: not <filename>\n");
            } else {
                char* argv[] = {"not", arg};
                not_start(2, argv, 1);
                clear_screen();
            }
        }
        else if (strcmp(command, "game") == 0) {
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
        }
        else if (strcmp(command, "info") == 0) {
            show_info(arg);
        }
        else if (strcmp(command, "log") == 0) {
            if (fs_create("inputs.txt", log_buffer) == 0) {
                print("The commands have been saved to the inputs.txt file..\n");
            } else {
                print("Failed to create file.\n");
            }
        }
        else if (strcmp(command, "colorbg") == 0) {
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
        }
        else if (strcmp(command, "ps") == 0) {
            process_list();
        }
        else if (strcmp(command, "pinfo") == 0) {
            if (strlen(arg) == 0) {
                print_color("Usage: pinfo <pid>\n", 0x0E);
            } else {
                int pid = simple_atoi(arg);
                process_info(pid);
            }
        }
        else if (strcmp(command, "pskill") == 0) {
            if (strlen(arg) == 0) {
                print_color("Usage: pskill <pid>\n", 0x0E);
            } else {
                int pid = simple_atoi(arg);
                int result = process_kill(pid);
                if (result == -1) {
                    print_color("Error: Process with PID ", 0x04);
                    print_int(pid);
                    print_color(" not found\n", 0x04);
                } else if (result == -2) {
                    print_color("Error: Cannot kill shell process\n", 0x04);
                }
            }
        }
        else if (strcmp(command, "clear") == 0) {
            clear_screen();
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
                if (c == 0x14) {
                    mini_terminal();
                    continue;
                }
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
                if (c == 0x14) {
                    mini_terminal();
                    continue;
                }
                if (c == ' ') break;
            }
            
            print_color("Process Management Commands:\n", 0x0D);
            print_color("  ps     -> Lists all processes\n", 0x07);
            print_color("  pinfo  -> Shows process information (pinfo <pid>)\n", 0x07);
            print_color("  pskill -> Terminates a process (pskill <pid>)\n", 0x07);
            print("\n");
            print_color("Press space for more...\n", 0x08);
            while (1) {
                char c = get_char();
                if (c == 0x14) {
                    mini_terminal();
                    continue;
                }
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
            print_color("  gui    -> Opens the GUI menu\n", 0x07);
            print("\n");
            print_color("Type any command to continue...\n", 0x08);
        }
        else if (strcmp(command, "gui") == 0) {
            draw_gui_menu();
        }
        else if (strcmp(command, "ls") == 0) {
            fs_list();
        }
        else if (strcmp(command, "cd") == 0) {
            if (strcmp(arg, "..") == 0 && current->parent != NULL) {
                current = current->parent;
            } else if (strlen(arg) == 0 || strcmp(arg, "~") == 0) {
                current = root;
            } else {
                Folder* next = find_subfolder(current, arg);
                if (next) current = next;
                else {
                    print("Folder not found: ");
                    print(arg);
                    print("\n");
                }
            }
        }
        else if (strcmp(command, "pwd") == 0) {
            print_path(current);
            print("\n");
        }
        else if (strcmp(command, "mkdir") == 0) {
            if (strlen(arg) == 0) {
                print("Usage: mkdir <name>\n");
            } else if (create_folder(arg, current)) {
                print("Folder created: ");
                print(arg);
                print("\n");
            } else {
                print("Folder could not be created (already exists or no space?)\n");
            }
        }
        else {
            print_color("Unknown command: ", 0x04);
            print(command);
            print("\n");
        }
        
        process_yield();
    }
}

void idle_entry() {

    static int initialized = 0;
    
    if (!initialized) {
        print_color("[IDLE] Idle process initialized (PID: ", 0x08);
        print_int(process_get_current_pid());
        print_color(")\n", 0x08);
        initialized = 1;
    }
    
    while (1) {
        for (volatile int i = 0; i < 1000000; i++) {
            asm volatile("nop");
        }
        process_yield();
    }
}

void kernel_main() {
    print("kernel active");
    set_vga_mode(TEXT_MODE);
    clear_screen();
    

    process_init();
    
    print_color("Enter your username: ",0x0A);
    gets(username);
    clear_screen();
    

    print_color("[KERNEL] Creating shell process...\\n", 0x0A);
    int shell_pid = process_create("shell", shell_entry, PRIORITY_HIGH);
    if (shell_pid > 0) {
        Process* shell_proc = process_get_by_pid(shell_pid);
        if (shell_proc) {
            shell_proc->state = PROCESS_READY;
        }
    }
    

    print_color("[KERNEL] Creating idle process...\\n", 0x0A);
    int idle_pid = process_create("idle", idle_entry, PRIORITY_LOW);
    if (idle_pid > 0) {
        Process* idle_proc = process_get_by_pid(idle_pid);
        if (idle_proc) {
            idle_proc->state = PROCESS_READY;
        }
    }
    
    print_color("[KERNEL] Process scheduler starting...\\n", 0x0A);
    print_color("[KERNEL] Type 'exit' to shutdown\\n\\n", 0x0A);
    

    while (1) {

        Process* current_proc = process_get_current();
        
        if (current_proc && current_proc->in_use) {
            if (current_proc->state == PROCESS_READY) {
                current_proc->state = PROCESS_RUNNING;
            }
            

            if (current_proc->entry_point && current_proc->state == PROCESS_RUNNING) {
                current_proc->entry_point();
            }
        }
        

        process_schedule();
    }
}
