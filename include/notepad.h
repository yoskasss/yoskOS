#ifndef NOTEPAD_H
#define NOTEPAD_H

extern void clear_screen(void);
extern void print(const char* str);
extern const char* fs_read(const char* name);
extern int fs_create(const char* name, const char* content);
extern char get_char(void);
extern void gets(char* buf);
extern char* strcpy(char* dest, const char* src);

void notepad_display(void);
void notepad_load(const char *fname);
void notepad_save(void);
int notepad_handle_input(int ch);
int not_start(int argc, char *argv[], int mode);

#endif 