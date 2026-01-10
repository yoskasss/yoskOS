#ifndef GAMES_H
#define GAMES_H

// Function declarations for games
void play_xox();
void play_hangman();

// External declarations for game variables
extern char xox_board[10];

// External function used by games
extern int simple_atoi(const char* str);

#endif // GAMES_H