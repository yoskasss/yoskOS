#include <stdbool.h> // <<< Burası düzeltildi!
#include <stddef.h> // size_t tipi için gerekli
#include <stdint.h>
#include "../include/string.h"  // Add string.h include
#include "../include/notepad.h" // Add notepad.h include
#include "../include/uptime.h"  // Add uptime.h include

// #include "string.h" // <<< Bu satır kaldırıldı! String fonksiyonlarını manuel olarak tanımlıyoruz.

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


// ----------------- Donanım Seviyesi Fonksiyonlar ------------------

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ----------------- Yardımcı Fonksiyonlar --------------------------


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

// RAM detection and usage functions
uint32_t get_total_ram_mb() {
    // For simplicity, we'll use a fixed value or detect from BIOS
    // In a real system, you'd read this from BIOS memory map
    // For now, we'll assume 512MB as a reasonable default
    return 512;
}

uint32_t get_used_ram_mb() {
    // Calculate used RAM by checking our data structures
    uint32_t used = 0;
    
    // File system usage
    used += sizeof(File) * MAX_FILES;
    
    // Folder system usage
    used += sizeof(Folder) * MAX_FOLDERS;
    
    // Buffer usage (notepad, etc.)
    used += 1000 * 256; // MAX_LINES * MAX_LINE_LENGTH from notepad
    
    // Convert to MB (rough approximation)
    return used / (1024 * 1024);
}

void print_ram_info() {
    uint32_t total_ram = get_total_ram_mb();
    uint32_t used_ram = get_used_ram_mb();
    uint32_t free_ram = total_ram - used_ram;
    
    // Calculate GB values (1 GB = 1024 MB)
    uint32_t total_ram_gb = total_ram / 1024;
    uint32_t used_ram_gb = used_ram / 1024;
    uint32_t free_ram_gb = free_ram / 1024;
    
    print("RAM Kullanimi: ");
    print_uint(used_ram);
    print(" MB (");
    print_uint(used_ram_gb);
    print(" GB) / ");
    print_uint(total_ram);
    print(" MB (");
    print_uint(total_ram_gb);
    print(" GB) (");
    print_uint(free_ram);
    print(" MB (");
    print_uint(free_ram_gb);
    print(" GB) bos)\n");
}

// Battery status functions
uint8_t get_battery_percentage() {
    // Simulated battery percentage
    // In a real system, you'd read this from ACPI or battery controller
    static uint8_t battery_level = 85; // Start at 85%
    
    // Simulate gradual battery drain over time
    static uint32_t last_check = 0;
    uint32_t current_time = (uint32_t)(get_uptime_seconds(1000000000ULL) / 60); // Minutes
    
    if (current_time > last_check + 5) { // Drain 1% every 5 minutes
        if (battery_level > 0) {
            battery_level--;
        }
        last_check = current_time;
    }
    
    return battery_level;
}

bool is_battery_charging() {
    // Simulated charging status
    // In a real system, you'd read this from ACPI
    static bool charging = false;
    static uint32_t last_check = 0;
    uint32_t current_time = (uint32_t)(get_uptime_seconds(1000000000ULL) / 30); // 30 second intervals
    
    if (current_time > last_check + 20) { // Toggle charging status every 10 minutes
        charging = !charging;
        last_check = current_time;
    }
    
    return charging;
}

uint32_t get_battery_time_remaining() {
    // Simulated time remaining in minutes
    uint8_t battery_level = get_battery_percentage();
    if (is_battery_charging()) {
        return 0; // Charging, no time remaining to show
    }
    
    // Rough estimate: 1% = 2 minutes of battery life
    return battery_level * 2;
}

void print_battery_info() {
    uint8_t battery_level = get_battery_percentage();
    bool charging = is_battery_charging();
    uint32_t time_remaining = get_battery_time_remaining();
    
    print("Batarya Durumu: ");
    print_uint(battery_level);
    print("%");
    
    if (charging) {
        print(" (Sarj oluyor)");
    } else {
        print(" (Sarj olmuyor)");
    }
    
    if (!charging && time_remaining > 0) {
        print(" - ");
        print_uint(time_remaining);
        print(" dakika kaldi");
    }
    
    print("\n");
}

// Storage/disk usage functions
uint32_t get_total_storage_gb() {
    // Simulated total storage capacity
    // In a real system, you'd read this from disk controller or filesystem
    return 256; // 256 GB total storage
}

uint32_t get_used_storage_mb() {
    // Calculate used storage by checking file system
    uint32_t used = 0;
    
    // Count used files and their content
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used) {
            // Add file name size
            used += strlen(fs[i].name);
            // Add file content size
            used += strlen(fs[i].content);
        }
    }
    
    // Add some overhead for filesystem structures
    used += sizeof(File) * MAX_FILES;
    used += sizeof(Folder) * MAX_FOLDERS;
    
    return used;
}

uint32_t get_free_storage_mb() {
    uint32_t total_mb = get_total_storage_gb() * 1024; // Convert GB to MB
    uint32_t used_mb = get_used_storage_mb();
    
    if (used_mb > total_mb) {
        return 0;
    }
    
    return total_mb - used_mb;
}

void print_storage_info() {
    uint32_t total_gb = get_total_storage_gb();
    uint32_t used_mb = get_used_storage_mb();
    uint32_t free_mb = get_free_storage_mb();
    
    // Calculate GB values
    uint32_t used_gb = used_mb / 1024;
    uint32_t free_gb = free_mb / 1024;
    
    print("Depolama Alani: ");
    print_uint(used_mb);
    print(" MB (");
    print_uint(used_gb);
    print(" GB) / ");
    print_uint(total_gb * 1024); // Total in MB
    print(" MB (");
    print_uint(total_gb);
    print(" GB) (");
    print_uint(free_mb);
    print(" MB (");
    print_uint(free_gb);
    print(" GB) bos)\n");
}

// Screen resolution functions
uint16_t current_width = 80;
uint16_t current_height = 25;

bool set_screen_resolution(uint16_t width, uint16_t height) {
    // Validate resolution limits
    if (width < 40 || width > 120 || height < 20 || height > 50) {
        return false; // Invalid resolution
    }
    
    // Update current resolution
    current_width = width;
    current_height = height;
    
    // Clear screen with new dimensions
    clear_screen();
    
    return true;
}

void get_current_resolution(uint16_t* width, uint16_t* height) {
    *width = current_width;
    *height = current_height;
}

void print_resolution_info() {
    print("Ekran Cozunurlugu: ");
    print_uint(current_width);
    print("x");
    print_uint(current_height);
    print("\n");
}

bool parse_resolution_command(const char* arg, uint16_t* width, uint16_t* height) {
    // Expected format: "ekran <width>x<height>"
    // Example: "ekran 100x30"
    
    const char* x_pos = strchr(arg, 'x');
    if (!x_pos) {
        return false; // No 'x' found
    }
    
    // Calculate positions
    int width_len = x_pos - arg;
    int height_len = strlen(x_pos + 1);
    
    if (width_len <= 0 || height_len <= 0) {
        return false; // Invalid format
    }
    
    // Extract width
    char width_str[10];
    if ((size_t)width_len >= sizeof(width_str)) {
        return false; // Width too long
    }
    strncpy(width_str, arg, width_len);
    width_str[width_len] = '\0';
    
    // Extract height
    char height_str[10];
    if ((size_t)height_len >= sizeof(height_str)) {
        return false; // Height too long
    }
    strcpy(height_str, x_pos + 1);
    
    // Convert to integers using simple conversion
    *width = 0;
    *height = 0;
    
    // Convert width
    for (int i = 0; width_str[i]; i++) {
        if (width_str[i] >= '0' && width_str[i] <= '9') {
            *width = *width * 10 + (width_str[i] - '0');
        }
    }
    
    // Convert height
    for (int i = 0; height_str[i]; i++) {
        if (height_str[i] >= '0' && height_str[i] <= '9') {
            *height = *height * 10 + (height_str[i] - '0');
        }
    }
    
    return true;
}

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
    for (int i = 0; i < 80 * 25; i++) {
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
                for (int i = 0; i < (80 * 24); i++) {
                    VIDEO_MEMORY[i] = VIDEO_MEMORY[i + 80];
                }
                // Clear the last line
                for (int i = (80 * 24); i < (80 * 25); i++) {
                    VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
                }
                cursor = 80 * 24; // Set cursor to the beginning of the last line
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
            for (int i = 0; i < (80 * 24); i++) {
                VIDEO_MEMORY[i] = VIDEO_MEMORY[i + 80];
            }
            // Clear the last line
            for (int i = (80 * 24); i < (80 * 25); i++) {
                VIDEO_MEMORY[i] = ' ' | (0x07 << 8);
            }
            cursor = 80 * 24; // Set cursor to the beginning of the last line
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
        if (strncmp(input, "ascii ", 6) == 0) { // strncmp artık tanımlı ve bağlı
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
        else if (strcmp(command, "sysinf") == 0){
            print("Acik kalma suresi\n");
            // Assume CPU frequency of 1 GHz for demonstration
            // In a real system, you'd detect this or use a timer
            print_uptime(1000000000ULL); // 1 GHz
            
            print("\n");
            print_ram_info();
            
            print("\n");
            print_storage_info();
            
            print("\n");
            print_battery_info();
        }
        else if (strcmp(command, "ekran") == 0) {
            if (strlen(arg) == 0) {
                // Show current resolution
                print_resolution_info();
            } else {
                // Parse and set new resolution
                uint16_t width, height;
                if (parse_resolution_command(arg, &width, &height)) {
                    if (set_screen_resolution(width, height)) {
                        print("Ekran cozunurlugu ");
                        print_uint(width);
                        print("x");
                        print_uint(height);
                        print(" olarak ayarlandi.\n");
                    } else {
                        print("Gecersiz cozunurluk! Gecerli aralik: 40-120x20-50\n");
                    }
                } else {
                    print("Kullanim: ekran <genislik>x<yukseklik>\n");
                    print("Ornek: ekran 100x30\n");
                }
            }
        }
        else if (strcmp(command, "ipinf") == 0) {
            print_network_info();
        }
        else if (strcmp(command, "ascii") == 0) {
            print("\n");
            print_ascii_art(arg);

        } else if (strcmp(command, "help") == 0) {
            //print("\nBu isletim sistemi HeJo tarafindan yapildi.\n");
            print("Komutlar:\n");
            print("  help   -> Bu menuyu acar\n");
            print("  ascii  -> ASCII art gosterir (ascii <yazi>)\n");
            print("  write  -> Bir dosya olusturur ve icine yazi yazar (write <dosyaadi> <icerik>)\n");
            print("  cat    -> Bir dosya icerigini gosterir (cat <dosyaadi>)\n");
            print("  mkdir  -> Klasor olusturur (mkdir <isim>)\n");
            print("  ls     -> Klasor icerigini listeler\n");
            print("  cd     -> Klasor degistir (cd <isim>) veya (cd ..)\n");
            print("  pwd    -> Bulundugun yolu gosterir\n");
            print("  not    -> Notepad uygulamasini acar\n");
            print("  sysinf -> Sistem bilgilerini gosterir\n");
            print("  ipinf  -> Ag bilgilerini gosterir (MAC, IPv4, IPv6)\n");
            print("  ekran  -> Ekran cozunurlugunu degistir (ekran <genislik>x<yukseklik>)\n");
            print("  clear  -> Ekrani temizler\n");

        } else if (strcmp(command, "clear") == 0) {
            clear_screen();

        } else {
            print("Bilinmeyen komut: ");
            print(command);
            print("\n");
        }
    }
}


// ----------------- Kernel Giriş -------------------------

void kernel_main() {
    clear_screen();
    
    // Initialize uptime timer
    start_uptime_timer();
    
    print("Kullanici adinizi girin: ");
    gets(username);
    clear_screen();
    run_terminal();
}

// Network information functions
void generate_mac_address(char* mac_str) {
    // Generate a realistic MAC address
    static uint8_t mac_counter = 0;
    mac_counter++;
    
    // Format: XX:XX:XX:XX:XX:XX
    const char hex_chars[] = "0123456789ABCDEF";
    int pos = 0;
    
    // 00:1B:44:11:22:XX
    mac_str[pos++] = '0';
    mac_str[pos++] = '0';
    mac_str[pos++] = ':';
    mac_str[pos++] = '1';
    mac_str[pos++] = 'B';
    mac_str[pos++] = ':';
    mac_str[pos++] = '4';
    mac_str[pos++] = '4';
    mac_str[pos++] = ':';
    mac_str[pos++] = '1';
    mac_str[pos++] = '1';
    mac_str[pos++] = ':';
    mac_str[pos++] = '2';
    mac_str[pos++] = '2';
    mac_str[pos++] = ':';
    mac_str[pos++] = hex_chars[(mac_counter >> 4) & 0xF];
    mac_str[pos++] = hex_chars[mac_counter & 0xF];
    mac_str[pos] = '\0';
}

void generate_ipv4_address(char* ip_str) {
    // Generate a realistic private IPv4 address
    static uint8_t ip_counter = 0;
    ip_counter++;
    
    // Format: 192.168.1.XXX
    int pos = 0;
    
    // 192.168.1.
    ip_str[pos++] = '1';
    ip_str[pos++] = '9';
    ip_str[pos++] = '2';
    ip_str[pos++] = '.';
    ip_str[pos++] = '1';
    ip_str[pos++] = '6';
    ip_str[pos++] = '8';
    ip_str[pos++] = '.';
    ip_str[pos++] = '1';
    ip_str[pos++] = '.';
    
    // Last octet (100-254)
    uint8_t last_octet = 100 + (ip_counter % 155);
    if (last_octet >= 100) {
        ip_str[pos++] = '1';
        ip_str[pos++] = '0';
        ip_str[pos++] = '0' + (last_octet - 100);
    } else {
        ip_str[pos++] = '0' + (last_octet / 10);
        ip_str[pos++] = '0' + (last_octet % 10);
    }
    ip_str[pos] = '\0';
}

void generate_ipv6_address(char* ipv6_str) {
    // Generate a realistic IPv6 address
    static uint8_t ipv6_counter = 0;
    ipv6_counter++;
    
    // Format: fe80::XXXX:XXXX:XXXX:XXXX
    int pos = 0;
    
    // fe80::
    ipv6_str[pos++] = 'f';
    ipv6_str[pos++] = 'e';
    ipv6_str[pos++] = '8';
    ipv6_str[pos++] = '0';
    ipv6_str[pos++] = ':';
    ipv6_str[pos++] = ':';
    
    // Generate 4 hex blocks
    const char hex_chars[] = "0123456789abcdef";
    for (int i = 0; i < 4; i++) {
        if (i > 0) ipv6_str[pos++] = ':';
        
        uint16_t block = 0x1234 + ipv6_counter * (i + 1);
        ipv6_str[pos++] = hex_chars[(block >> 12) & 0xF];
        ipv6_str[pos++] = hex_chars[(block >> 8) & 0xF];
        ipv6_str[pos++] = hex_chars[(block >> 4) & 0xF];
        ipv6_str[pos++] = hex_chars[block & 0xF];
    }
    ipv6_str[pos] = '\0';
}

void print_network_info() {
    char mac_addr[18];
    char ipv4_addr[16];
    char ipv6_addr[40];
    
    generate_mac_address(mac_addr);
    generate_ipv4_address(ipv4_addr);
    generate_ipv6_address(ipv6_addr);
    
    print("Ag Bilgileri:\n");
    print("  MAC Adresi: ");
    print(mac_addr);
    print("\n");
    print("  IPv4 Adresi: ");
    print(ipv4_addr);
    print("\n");
    print("  IPv6 Adresi: ");
    print(ipv6_addr);
    print("\n");
    print("  Ag Durumu: Aktif\n");
    print("  Baglanti Tipi: Ethernet\n");
}
