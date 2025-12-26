#include <stdbool.h> // <<< Burası düzeltildi!
#include <stddef.h> // size_t tipi için gerekli
#include <stdint.h>
#include "../include/string.h"  // Add string.h include
#include "../include/notepad.h" // Add notepad.h include
#include "../include/uptime.h"  // Add uptime.h include
#include "../include/images.h"  // Add images.h include


// Global screen dimensions
int screen_width = 80;
int screen_height = 25;

// Function declarations
// String fonksiyonlarının bildirimleri (artık kernel.c içinde tanımlanacakları için)
char* strcpy(char* dest, const char* src);
int not_start(int argc, char *argv[]);
void print(const char* str);
char get_char();
void clear_screen();
void print_ascii_art(const char* text);
void parse_input(const char* input, char* command, char* arg);
void get_line(char* buffer, int max_len);
void gets(char* buf);
void run_terminal();

// Additional function declarations
void print_uint(uint64_t num);
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

// Global variables for keyboard state
static bool shift_active = false;
static bool caps_lock_active = false;
static bool ctrl_active = false;

volatile unsigned short* VIDEO_MEMORY = (unsigned short*) 0xB8000;
int cursor = 0;
char username[32];
const char* ascii_letters[26][5] = {
    // A
    {
        "  A  ",
        " A A ",
        "AAAAA",
        "A   A",
        "A   A"
    },
    // B
    {
        "BBBB ",
        "B   B",
        "BBBB ",
        "B   B",
        "BBBB "
    },
    // C
    {
        " CCCC",
        "C    ",
        "C    ",
        "C    ",
        " CCCC"
    },
    // D
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
    // G (Placeholders for ASCII art - Turkish chars won't render unless custom font is loaded)
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

// Türkçe Q klavye için scancode → karakter haritası (unshifted)
// Bu karakterler ISO-8859-9 (Latin-5) kodlamasına göre ayarlanmıştır.
// Kaynak dosyanızın herhangi bir kodlamada (UTF-8 dahil) kaydedilmesi sorun yaratmaz
// çünkü karakter değerleri doğrudan hex kaçış dizileri ile belirtilmiştir.
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

// Türkçe Q klavye için scancode → karakter haritası (shifted)
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

// ------------ VGA Grafik Fonksiyonları -------------

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define TEXT_MODE 0x03
#define GRAPHICS_MODE 0x13

// VGA modunu ayarlamak için assembly fonksiyonu
void set_vga_mode(int mode) {
    // BIOS interrupts (int $0x10) are not available when the kernel
    // is loaded in protected mode by GRUB. Calling int $0x10 here will
    // cause a crash because the IDT/interrupt handlers are not set up.
    // Make this function a no-op to avoid early kernel crashes.
    (void)mode;
}

// Grafik modunda ekrana bir piksel çizer
void draw_pixel(int x, int y, unsigned char color) {
    // Sınır kontrolü
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return;
    }
    unsigned char* vga_memory = (unsigned char*) 0xA0000;
    vga_memory[y * VGA_WIDTH + x] = color;
}

// Ham bitmap verisini ekrana çizer
void draw_bitmap(const unsigned char* data, int x, int y, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, data[i * width + j]);
        }
    }
}

// Grafik modunda ekranı temizler
void clear_graphics_screen(unsigned char color) {
    unsigned char* vga_memory = (unsigned char*) 0xA0000;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_memory[i] = color;
    }
}

// ----------------- Donanım Seviyesi Fonksiyonlar ------------------

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ----------------- Yardımcı Fonksiyonlar --------------------------
// Tek bir karakteri ekrana basan fonksiyon
void print_char(char c) {
    // VIDEO_MEMORY ve cursor değişkenlerinizin burada erişilebilir olduğunu varsayıyorum
    // Bu kısım sizin kernel yapınıza göre değişebilir
    VIDEO_MEMORY[cursor * 2] = c;
    VIDEO_MEMORY[cursor * 2 + 1] = 0x0F; // Beyaz yazı, siyah arka plan
    cursor++;
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
    
    // Sayıyı ters çevirmeden yazdırmak için özyinelemeli (recursive) yaklaşım
    if (n / 10) {
        print_int(n / 10);
    }
    print_char((n % 10) + '0');
}
// Basit bir atoi (ASCII to Integer) fonksiyonu
// Sadece pozitif sayıları destekler.
int simple_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + str[i] - '0';
        } else {
            // Sayısal olmayan bir karakterle karşılaşılırsa dur
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
            return NULL; // Zaten var
    }

    Folder* new_folder = allocate_folder();
    if (!new_folder) return NULL;

    strncpy(new_folder->name, name, MAX_NAME_LEN);
    new_folder->name[MAX_NAME_LEN - 1] = '\0'; // Ensure null termination
    new_folder->parent = parent;
    new_folder->child_count = 0;

    parent->children[parent->child_count++] = new_folder;
    return new_folder;
}

void list_folder(Folder* folder) {
    if (folder->child_count == 0) {
        print("Klasör boş.\n");
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
        // This is the root, print the base
        // If your root is "/", you'd print "/" here
        // If it's "YoskHome", it's the first part of the path
        print(""); // Handled by first / below
    }
    print("/");
    print(folder->name);
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
    
    // Check if file already exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used && strcmp(fs[i].name, name) == 0) {
            // Update existing file
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

void clear_screen() {
    for (int i = 0; i < screen_width * screen_height; i++) {
        VIDEO_MEMORY[i] = ' ' | (0x07 << 8); // White on black
    }
    cursor = 0;
}
void print_ascii_art(const char* text) {
    // 5 satırlık art
    for (int line = 0; line < 5; line++) {
        const char* p = text;
        while (*p) {
            char c = *p;
            // Türkçe karakterler için ASCII art desteği YOKTUR.
            // Bu kısım sadece Latin alfabesi A-Z için çalışır.
            // ISO-8859-9'dan ASCII'ye çevirip büyüğe çevirelim
            if (c >= 'a' && c <= 'z') c -= 32; // küçükse büyüğe çevir

            // Convert Turkish characters to their closest ASCII equivalents for ASCII art
            // This is a simplification; a full ASCII art system would need custom glyphs
            if (c == '\xD0') c = 'G'; // Ğ to G
            if (c == '\xA6') c = 'S'; // Ş to S
            if (c == '\xDD') c = 'I'; // İ to I
            if (c == '\xD6') c = 'O'; // Ö to O
            if (c == '\xDC') c = 'U'; // Ü to U
            if (c == '\xC7') c = 'C'; // Ç to C

            if (c >= 'A' && c <= 'Z') {
                int idx = c - 'A';
                print(ascii_letters[idx][line]);
            } else if (c == ' ') {
                print("     "); // boşluk için boşluk bırak
            } else {
                // Bilinmeyen veya Türkçe karakterler için boşluk
                print("     ");
            }
            print(" "); // harfler arası boşluk
            p++;
        }
        print("\n");
    }
}

// Render the grayscale bitmap as ASCII art in text mode.
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
            // Average brightness in the block
            unsigned int sum = 0;
            unsigned int count = 0;
            for (int y = by * block_h; y < (by + 1) * block_h && y < height; y++) {
                for (int x = bx * block_w; x < (bx + 1) * block_w && x < width; x++) {
                    sum += data[y * width + x];
                    count++;
                }
            }
            unsigned char avg = (count == 0) ? 0 : (unsigned char)(sum / count);

            // Map avg (0..255) to ramp index
            int idx = (avg * (ramp_len - 1)) / 255;
            char ch = ramp[idx];
            char s[2] = {ch, '\0'};
            print(s);
        }
        print("\n");
    }
}

// Render bitmap into VGA text mode by mapping average brightness to VGA colors.
// This uses the text-mode framebuffer at 0xB8000: each cell is 2 bytes (char + attr).
// Standart VGA/Konsol 16 Renginin RGB Değerleri (0-255)
// Bu palet, orijinal renge en yakın konsol rengini bulmak için kullanılacak.
const unsigned char vga_palette[16][3] = {
    {0, 0, 0},        // 0: Siyah
    {0, 0, 170},      // 1: Mavi
    {0, 170, 0},      // 2: Yeşil
    {0, 170, 170},    // 3: Camgöbeği (Cyan)
    {170, 0, 0},      // 4: Kırmızı
    {170, 0, 170},    // 5: Eflatun (Magenta)
    {170, 85, 0},     // 6: Kahverengi (Sarı)
    {170, 170, 170},  // 7: Açık Gri
    {85, 85, 85},     // 8: Koyu Gri
    {85, 85, 255},    // 9: Parlak Mavi
    {85, 255, 85},    // 10: Parlak Yeşil
    {85, 255, 255},   // 11: Parlak Camgöbeği
    {255, 85, 85},    // 12: Parlak Kırmızı
    {255, 85, 255},   // 13: Parlak Eflatun
    {255, 255, 85},   // 14: Parlak Sarı
    {255, 255, 255}   // 15: Beyaz
};

// Verilen bir RGB rengine en yakın VGA palet renginin indeksini bulan fonksiyon
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

    // Compute output size that fits the screen while preserving aspect ratio
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
            // ...compute the corresponding input rectangle
            int x0 = (bx * width) / out_w;
            int x1 = ((bx + 1) * width) / out_w;
            int y0 = (by * height) / out_h;
            int y1 = ((by + 1) * height) / out_h;

            // *** DEĞİŞİKLİK BAŞLANGICI: Ortalama RGB rengini hesapla ***
            unsigned long long sum_r = 0, sum_g = 0, sum_b = 0;
            int count = 0;
            for (int y = y0; y < y1; y++) {
                for (int x = x0; x < x1; x++) {
                    // Veri dizisi artık RGB olduğu için her piksel 3 bayttır
                    int pixel_index = (y * width + x) * 3;
                    sum_r += data[pixel_index + 0]; // Kırmızı
                    sum_g += data[pixel_index + 1]; // Yeşil
                    sum_b += data[pixel_index + 2]; // Mavi
                    count++;
                }
            }
            
            unsigned char avg_r = (count == 0) ? 0 : (unsigned char)(sum_r / count);
            unsigned char avg_g = (count == 0) ? 0 : (unsigned char)(sum_g / count);
            unsigned char avg_b = (count == 0) ? 0 : (unsigned char)(sum_b / count);

            // Ortalama renge en yakın 16 konsol rengini bul
            int color_index = find_closest_vga_color(avg_r, avg_g, avg_b);
            // *** DEĞİŞİKLİK SONU ***


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
    const int margin_right = 30; // sağdan boşluk

    // Ölçekleme (aynı)
    int out_w = max_w;
    int out_h = (height * out_w) / width;
    if (out_h > max_h) {
        out_h = max_h;
        out_w = (width * out_h) / height;
    }
    if (out_w < 1) out_w = 1;
    if (out_h < 1) out_h = 1;

    // === SAĞA YASLI OFFSET ===
    int offset_x = screen_w - out_w - margin_right;
    if (offset_x < 0) offset_x = 0;

    int offset_y = 0; // üstte

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
void print_color(const char* str, uint8_t color) {
    while (*str) {
        unsigned char c = (unsigned char)*str;

        // Handle newline
        if (c == '\n') {
            cursor += 80 - (cursor % 80);
            str++;
            continue;
        } else if (c == '\r') {
            cursor = (cursor / 80) * 80;
            str++;
            continue;
        } else if (c == '\b') {
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | (color << 8);
            }
            str++;
            continue;
        }

        // ASCII characters
        if (c < 128) {
            VIDEO_MEMORY[cursor++] = c | (color << 8);
            str++;

            // Scroll
            if (cursor >= screen_width * screen_height) {
                for (int i = 0; i < screen_width * 24; i++)
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                for (int i = screen_width * 24; i < screen_width * screen_height; i++)
                    VIDEO_MEMORY[i] = ' ' | (color << 8);
                cursor = screen_width * 24;
            }
            continue;
        }

        // UTF-8 Turkish characters
        if (c == 0xC3) {
            str++;
            unsigned char next_byte = (unsigned char)*str;
            char display_char = '?';
            switch (next_byte) {
                case 0x96: display_char = '\xD6'; break; // Ö
                case 0x9C: display_char = '\xDC'; break; // Ü
                case 0xA7: display_char = '\xE7'; break; // ç
                case 0xB6: display_char = '\xF6'; break; // ö
                case 0xBC: display_char = '\xFC'; break; // ü
            }
            VIDEO_MEMORY[cursor++] = display_char | (color << 8);
            str++;
        } else if (c == 0xC4) {
            str++;
            unsigned char next_byte = (unsigned char)*str;
            char display_char = '?';
            switch (next_byte) {
                case 0x9E: display_char = '\xD0'; break; // Ğ
                case 0xB0: display_char = '\xDD'; break; // İ
                case 0xB1: display_char = '\xEF'; break; // ı
                case 0x9F: display_char = '\xF0'; break; // ğ
            }
            VIDEO_MEMORY[cursor++] = display_char | (color << 8);
            str++;
        } else if (c == 0xC5) {
            str++;
            unsigned char next_byte = (unsigned char)*str;
            char display_char = '?';
            switch (next_byte) {
                case 0x9E: display_char = '\xA6'; break; // Ş
                case 0x9F: display_char = '\xB6'; break; // ş
            }
            VIDEO_MEMORY[cursor++] = display_char | (color << 8);
            str++;
        } else {
            VIDEO_MEMORY[cursor++] = '?' | (color << 8);
            str++;
        }

        if (cursor >= screen_width * screen_height) {
            for (int i = 0; i < screen_width * 24; i++)
                VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
            for (int i = screen_width * 24; i < screen_width * screen_height; i++)
                VIDEO_MEMORY[i] = ' ' | (color << 8);
            cursor = screen_width * 24;
        }
    }
}


void print(const char* str) {
    while (*str) {
        unsigned char c = (unsigned char)*str;

        // Handle newline, carriage return, backspace (single-byte characters)
        if (c == '\n') {
            cursor += 80 - (cursor % 80); // Move to the start of the next line
            str++;
            continue;
        } else if (c == '\r') {
            cursor = (cursor / 80) * 80; // Move to the start of the current line
            str++;
            continue;
        } else if (c == '\b') { // Backspace
            if (cursor > 0) {
                cursor--;
                VIDEO_MEMORY[cursor] = ' ' | (0x07 << 8); // Erase char with default attribute
            }
            str++;
            continue;
        }

        // Handle ASCII characters (0-127)
        if (c < 128) {
            // Debug: Print ASCII character and its value
            // char debug_msg[16];
            // sprintf(debug_msg, "ASCII: %c (%x)\n", c, c);
            // print_debug(debug_msg); // Assuming a debug print function exists

            VIDEO_MEMORY[cursor++] = c | (0x07 << 8); // Write char with default attribute
            str++;
            // Scroll if necessary (check after printing a character)
            if (cursor >= 80 * 25) {
                // Move all lines up by one
                for (int i = 0; i < screen_width * 24; i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
                }
                // Clear the last line
                for (int i = (screen_width * 24); i < (screen_width * screen_height); i++) {
                    VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
                }
                cursor = screen_width * 24; // Set cursor to the beginning of the last line
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
                // Debug: Print UTF-8 sequence and mapped character
                // char debug_msg[32];
                // sprintf(debug_msg, "UTF8 C3 %x -> %c (%x)\n", next_byte, display_char, display_char);
                // print_debug(debug_msg); // Assuming a debug print function exists

                 VIDEO_MEMORY[cursor++] = display_char | (0x07 << 8);
                 str++;
            } else {
                // Incomplete sequence, print placeholder
                // Debug: Print incomplete UTF-8 sequence
                // char debug_msg[16];
                // sprintf(debug_msg, "UTF8 Incomplete C3\n");
                // print_debug(debug_msg); // Assuming a debug print function exists

                VIDEO_MEMORY[cursor++] = '?' | (0x07 << 8);
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
                 // Debug: Print UTF-8 sequence and mapped character
                // char debug_msg[32];
                // sprintf(debug_msg, "UTF8 C4 %x -> %c (%x)\n", next_byte, display_char, display_char);
                // print_debug(debug_msg); // Assuming a debug print function exists

                 VIDEO_MEMORY[cursor++] = display_char | (0x07 << 8);
                 str++;
            } else {
                // Debug: Print incomplete UTF-8 sequence
                // char debug_msg[16];
                // sprintf(debug_msg, "UTF8 Incomplete C4\n");
                // print_debug(debug_msg); // Assuming a debug print function exists

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
                 // Debug: Print UTF-8 sequence and mapped character
                // char debug_msg[32];
                // sprintf(debug_msg, "UTF8 C5 %x -> %c (%x)\n", next_byte, display_char, display_char);
                // print_debug(debug_msg); // Assuming a debug print function exists

                 VIDEO_MEMORY[cursor++] = display_char | (0x07 << 8);
                 str++;
            } else {
                // Debug: Print incomplete UTF-8 sequence
                // char debug_msg[16];
                // sprintf(debug_msg, "UTF8 Incomplete C5\n");
                // print_debug(debug_msg); // Assuming a debug print function exists

                 VIDEO_MEMORY[cursor++] = '?' | (0x07 << 8);
            }
        } else { // Unhandled multi-byte start or invalid UTF-8
            // Debug: Print unhandled byte
            // char debug_msg[16];
            // sprintf(debug_msg, "Unhandled byte %x\n", c);
            // print_debug(debug_msg); // Assuming a debug print function exists

            VIDEO_MEMORY[cursor++] = '?' | (0x07 << 8); // Print placeholder
            str++; // Skip the potentially invalid byte
        }

        // Scroll if necessary (check after printing a character)
        if (cursor >= 80 * 25) {
            // Move all lines up by one
            for (int i = 0; i < screen_width * 24; i++) {
                VIDEO_MEMORY[i] = VIDEO_MEMORY[i + screen_width];
            }
            // Clear the last line
            for (int i = (screen_width * 24); i < (screen_width * screen_height); i++) {
                VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
            }
            cursor = screen_width * 24; // Set cursor to the beginning of the last line
        }
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
void print_uint(uint64_t num) {
    char buf[20];
    int i = 0;
    if (num == 0) {
        putchar('0');
        return;
    }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i--) {
        putchar(buf[i]);
    }
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
// ----------------- Terminal ---------------------------

void run_terminal() {
    char command[20];
    char arg[80];

    // Root klasör
    Folder* root = allocate_folder();
    strcpy(root->name, "YoskHome"); // Renamed root to YoskHome
    root->parent = NULL;
    root->child_count = 0;

    Folder* current = root;
    char input[128]; // Increased input buffer size

    print_ascii_art("\nyoskOS\n\n");
    print("Temizlemek icin 'clear' yaz\n");

    while (1) {
        print("\nYoskShell&");
        print(username);
        print(" -> ");

        get_line(input, sizeof(input));
        parse_input(input, command, arg);

        if (input[0] == '\0') continue;
        // run_terminal fonksiyonu içindeki if bloğunun son hali

          if (strcmp(command, "show") == 0) {
            print("Kullanilabilir resimler: 1-2-3 ");
            print_int(num_available_images);
            print("\nGosterilecek resim numarasini girin: ");

            char choice_buffer[128]; // gets fonksiyonunuz 128'lik buffer bekliyor gibi
            gets(choice_buffer);     // SİZİN FONKSİYONUNUZU ÇAĞIRIYORUZ
            
            int image_num = simple_atoi(choice_buffer);

            if (image_num > 0 && image_num <= num_available_images) {
                const Image selected_image = available_images[image_num - 1];

                clear_screen();
                render_bitmap_vga_text(selected_image.data, selected_image.width, selected_image.height);
                
                cursor = 80 * 24;
                print("Vizuel gosterim tamam. Tekrar devam etmek icin bir tusa basin...");
                
                // Buradaki wait_for_keypress'in `void` döndüren versiyonu
                // Sadece bir tuşa basılmasını bekler, karakteri almaz.
                wait_for_keypress(); 
                
                clear_screen();
                print_ascii_art("\nyoskOS\n\n");
                print("Temizlemek icin 'clear' yaz\n");
            } else {
                print("Hata: Gecersiz veya bulunamayan numara.\n");
            }
        }
        else if (strncmp(input, "ascii ", 6) == 0) { // strncmp artık tanımlı ve bağlı
            print("\n");
            print_ascii_art(input + 6);
        }
        
        else if (strcmp(command, "mkdir") == 0) {
            if (strlen(arg) == 0) {
                print("Kullanım: mkdir <isim>\n");
                continue;
            }
            if (create_folder(arg, current)) {
                print("Klasör oluşturuldu: ");
                print(arg);
                print("\n");
            } else {
                print("Klasör oluşturulamadı (zaten var mı ya da yer yok mu?)\n");
            }

        }
        // run_terminal fonksiyonunda not_start çağrısını düzeltin
        else if (strcmp(command, "not") == 0) {
            if (strlen(arg) == 0) {
                print("Kullanım: not <dosyaadı>\n");
                continue;
            }
            char *args[] = {"not", arg};  // Argüman dizisi oluştur
            not_start(2, args);  // 2 argümanla çağır (program adı ve dosya adı)
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
                    print("Klasör bulunamadı: ");
                    print(arg);
                    print("\n");
                }
            }

        }
        else if (strncmp(input, "write ", 6) == 0) { // strncmp artık tanımlı ve bağlı
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
                    print("Kullanım: write <dosyaadi> <icerik>\n");
                } else if (fs_create(name_start, content_start) == 0) {
                    print("Dosya yazıldı.\n");
                } else {
                    print("Dosya yazılamadı (yer yok mu?)\n");
                }
            } else {
                print("Kullanım: write <dosyaadi> <icerik>\n");
            }
        }
        else if (strcmp(command, "cat") == 0) {
            if (strlen(arg) == 0) {
                print("Kullanım: cat <dosyaadi>\n");
                continue;
            }
            const char* content = fs_read(arg);
            if (content) {
                print(content);
                print("\n");
            } else {
                print("Dosya bulunamadı.\n");
            }
        }
        else if (strcmp(command, "pwd") == 0) {
            print_path(current);
            print("\n");

        }

        else if (strcmp(command, "ascii") == 0) {
            print("\n");
            print_ascii_art(arg);
        }
        else if (strcmp(command, "help") == 0) {
            clear_screen();
            // Üst tarafta küçük logo göster
            render_logo_at_top(available_images[3].data, available_images[3].width, available_images[3].height, 80, 25);
            cursor = screen_width * 0 + 11; // Logo 10 genişliğinde, yanında başla
            
            // Cursor'u logo altına ayarla
            cursor = screen_width * 17;
            print("Komutlar:\n");
            print_color("  help   -> Bu menuyu acar\n",0x0A);    
            print_color("  ascii  -> ASCII art gosterir (ascii <yazi>)\n",0x0A);
            print_color("  write  -> Bir dosya olusturur ve icine yazi yazar (write <dosyaadi> <icerik>)\n",0x0A);
            print_color("  cat    -> Bir dosya icerigini gosterir (cat <dosyaadi>)\n",0x0A);
            print_color("  mkdir  -> Klasor olusturur (mkdir <isim>)\n",0x0A);
            print_color("  ls     -> Klasor icerigini listeler\n",0x0A);
            print_color("  cd     -> Klasor degistir (cd <isim>) veya (cd ..)\n",0x0A);
            print_color("  pwd    -> Bulundugun yolu gosterir\n",0x0A);
            print_color("  not    -> Notepad uygulamasini acar\n",0x0A);
            print_color("  game   -> Oyun uygulamasini acar\n",0x0A);
            print_color("  show    -> Bitmap ile görsel görüntüleyici\n",0X0A);
            print_color("  clear  -> Ekrani temizler\n",0X0A);

        } else if (strcmp(command, "clear") == 0) {
            clear_screen();

        } else if (strcmp(command, "game") == 0) {
            clear_screen();
            print("Oyunlar:\n");
            print("1. XOX\n");
            print("2. Hangman\n");
            print("Secim yapin: ");
            char game_input[128];
            get_line(game_input, sizeof(game_input));
            int choice = simple_atoi(game_input);
            if (choice == 1) {
                play_xox();
            } else if (choice == 2) {
                play_hangman();
            } else {
                print("Gecersiz secim\n");
            }
            print("Oyundan cikmak icin bir tusa basin...");
            wait_for_keypress();
            clear_screen();
        } else {
            print_color("Bilinmeyen komut: ",0X04);
            print(command);
            print("\n");
        }
    }
}


// Oyunlar için yardımcı fonksiyonlar
// (print_char ve print_int zaten tanımlı)

// XOX Oyunu
int xox_board[10] = {0,1,2,3,4,5,6,7,8,9};

void drawBoard() {
    print("\n");
    print(" ");
    print_int(xox_board[1]);
    print(" | ");
    print_int(xox_board[2]);
    print(" | ");
    print_int(xox_board[3]);
    print(" \n");
    print("---+---+---\n");
    print(" ");
    print_int(xox_board[4]);
    print(" | ");
    print_int(xox_board[5]);
    print(" | ");
    print_int(xox_board[6]);
    print(" \n");
    print("---+---+---\n");
    print(" ");
    print_int(xox_board[7]);
    print(" | ");
    print_int(xox_board[8]);
    print(" | ");
    print_int(xox_board[9]);
    print(" \n\n");
}

int checkWin() {
    int wins[8][3] = {
        {1,2,3},{4,5,6},{7,8,9},
        {1,4,7},{2,5,8},{3,6,9},
        {1,5,9},{3,5,7}
    };
    int i;
    for (i = 0; i < 8; i++) {
        if (xox_board[wins[i][0]] == xox_board[wins[i][1]] &&
            xox_board[wins[i][1]] == xox_board[wins[i][2]])
            return 1;
    }
    return 0;
}

int isDraw() {
    int i;
    for (i = 1; i <= 9; i++) {
        if (xox_board[i] != 'X' && xox_board[i] != 'O')
            return 0;
    }
    return 1;
}

int findMove(int mark) {
    int i;
    for (i = 1; i <= 9; i++) {
        if (xox_board[i] != 'X' && xox_board[i] != 'O') {
            int temp = xox_board[i];
            xox_board[i] = mark;
            if (checkWin()) {
                xox_board[i] = temp;
                return i;
            }
            xox_board[i] = temp;
        }
    }
    return 0;
}

int computerMove() {
    int move;

    /* 1. Try to win */
    move = findMove('O');
    if (move) return move;

    /* 2. Block player */
    move = findMove('X');
    if (move) return move;

    /* 3. Take first available */
    int i;
    for (i = 1; i <= 9; i++) {
        if (xox_board[i] != 'X' && xox_board[i] != 'O')
            return i;
    }

    return 0;
}

void resetBoard() {
    int i;
    for (i = 1; i <= 9; i++)
        xox_board[i] = i;
}

void play_xox() {
    int mode;
    int player = 1;
    int choice;
    char input[128];

    print("=== TIC TAC TOE ===\n");
    print("1. Player vs Player\n");
    print("2. Player vs Computer\n");
    print("Choose mode: ");
    get_line(input, sizeof(input));
    mode = simple_atoi(input);

    resetBoard();

    while (1) {
        drawBoard();

        if (mode == 2 && player == 2) {
            choice = computerMove();
            print("Computer chooses: ");
            print_int(choice);
            print("\n");
        } else {
            print("Player ");
            print_int(player);
            print(" (");
            print_char((player == 1) ? 'X' : 'O');
            print("), enter position (1-9): ");
            get_line(input, sizeof(input));
            choice = simple_atoi(input);
        }

        if (choice < 1 || choice > 9 ||
            xox_board[choice] == 'X' || xox_board[choice] == 'O') {
            print("Invalid move! Try again.\n");
            continue;
        }

        xox_board[choice] = (player == 1) ? 'X' : 'O';

        if (checkWin()) {
            drawBoard();
            if (mode == 2 && player == 2)
                print("Computer wins!\n");
            else {
                print("Player ");
                print_int(player);
                print(" wins!\n");
            }
            break;
        }

        if (isDraw()) {
            drawBoard();
            print("It's a draw!\n");
            break;
        }

        player = (player == 1) ? 2 : 1;
    }
}

// Hangman Oyunu
#define MAX_LIVES 6
#define MAX_WORD 20
#define WORD_COUNT 6

char toUpper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

void drawHangman(int lives) {
    print("\n");
    if (lives <= 5) print(" O \n"); else print("\n");
    if (lives <= 4) print("/"); else print(" ");
    if (lives <= 3) print("|"); else print(" ");
    if (lives <= 2) print("\\\n"); else print("\n");
    if (lives <= 1) print("/ "); else print("  ");
    if (lives <= 0) print("\\\n"); else print("\n");
}

void getAutoWord(char word[]) {
    static int index = 0;
    char words[WORD_COUNT][MAX_WORD] = {
        "PROGRAMMING",
        "COMPUTER",
        "HANGMAN",
        "LANGUAGE",
        "KEYBOARD",
        "SOFTWARE"
    };

    int i;
    for (i = 0; words[index][i] != '\0'; i++)
        word[i] = words[index][i];
    word[i] = '\0';

    index++;
    if (index >= WORD_COUNT)
        index = 0;
}

void play_hangman() {
    char secret[MAX_WORD];
    char guessed[MAX_WORD];
    char used[26];
    int usedCount = 0;

    int lives = MAX_LIVES;
    int mode;
    int i, correct, win = 0;
    char guess;
    char input[128];

    print("=== HANGMAN GAME ===\n");
    print("1. Single Player\n");
    print("2. Two Player\n");
    print("Choose mode: ");
    get_line(input, sizeof(input));
    mode = simple_atoi(input);

    if (mode == 1) {
        getAutoWord(secret);
    } else if (mode == 2) {
        print("Player 1, enter the secret word: ");
        get_line(input, sizeof(input));
        for (i = 0; input[i] != '\0' && input[i] != '\n'; i++)
            secret[i] = toUpper(input[i]);
        secret[i] = '\0';
        print("\n\n\n\n\n"); /* hide word */
    } else {
        print("Invalid mode!\n");
        return;
    }

    for (i = 0; secret[i] != '\0'; i++)
        guessed[i] = '_';
    guessed[i] = '\0';

    print("Player 2, start guessing!\n");

    while (lives > 0 && !win) {
        print("\nWord: ");
        print(guessed);
        print("\nLives left: ");
        print_int(lives);
        print("\n");
        drawHangman(lives);

        print("Used letters: ");
        for (i = 0; i < usedCount; i++) {
            print_char(used[i]);
            print(" ");
        }
        print("\n");

        print("Enter a letter: ");
        get_line(input, sizeof(input));
        guess = input[0];

        guess = toUpper(guess);

        int already_used = 0;
        for (i = 0; i < usedCount; i++) {
            if (used[i] == guess) {
                print("Letter already used!\n");
                already_used = 1;
                break;
            }
        }
        if (already_used)
            continue;

        used[usedCount++] = guess;

        correct = 0;
        for (i = 0; secret[i] != '\0'; i++) {
            if (secret[i] == guess) {
                guessed[i] = guess;
                correct = 1;
            }
        }

        if (!correct) {
            lives--;
            print("Wrong guess!\n");
        }

        win = 1;
        for (i = 0; guessed[i] != '\0'; i++) {
            if (guessed[i] == '_') {
                win = 0;
                break;
            }
        }
    }

    if (win) {
        print("\nYOU WIN!\n");
    } else {
        drawHangman(lives);
        print("\nGAME OVER!\n");
    }

    print("The word was: ");
    print(secret);
    print("\n");
}

// ----------------- Kernel Giriş -------------------------

void kernel_main() {
    print("kernel aktif");
    set_vga_mode(TEXT_MODE);
    clear_screen();
    print_color("Kullanici adinizi girin: ",0x0A);
    gets(username);
    clear_screen();
    run_terminal();
}

