// fs.c - RAM içinde basit dosya sistemi
#include "fs.h"
#include "screen.h"
#include "string.h"

File files[MAX_FILES];
int file_count = 0;

void fs_init() {
    file_count = 0;
}

int fs_create(char *name, char *content) {
    if (file_count >= MAX_FILES)
        return -1;

    strncpy(files[file_count].name, name, MAX_NAME_LEN);
    strncpy(files[file_count].content, content, MAX_CONTENT_LEN);
    file_count++;
    return 0;
}

char* fs_read(char *name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            return files[i].content;
        }
    }
    return NULL;
}

void fs_list() {
    print("Dosyalar:\n");
    for (int i = 0; i < file_count; i++) {
        print(" - ");
        print(files[i].name);
        print("\n");
    }
}
