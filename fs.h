#ifndef FS_H
#define FS_H

#define MAX_FILES 16
#define MAX_NAME_LEN 32
#define MAX_CONTENT_LEN 256

typedef struct {
    char name[MAX_NAME_LEN];
    char content[MAX_CONTENT_LEN];
} File;

void fs_init();
int fs_create(char *name, char *content);
char* fs_read(char *name);
void fs_list();

#endif
