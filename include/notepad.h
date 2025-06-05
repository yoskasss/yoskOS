#ifndef NOTEPAD_H
#define NOTEPAD_H

// Notepad function declarations
void notepad_display(void);
void notepad_load(const char *fname);
void notepad_save(void);
void notepad_handle_input(int ch);
int not_start(int argc, char *argv[]);

#endif 