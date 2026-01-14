#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/games.h"
#include "../include/console.h"
#include "../include/string.h"

char xox_board[10] = {'0','1','2','3','4','5','6','7','8','9'};

void drawBoard() {
    print("\n");
    print(" ");
    if (xox_board[1] == 'X' || xox_board[1] == 'O') print_char(xox_board[1]); else print_int(xox_board[1] - '0');
    print(" | ");
    if (xox_board[2] == 'X' || xox_board[2] == 'O') print_char(xox_board[2]); else print_int(xox_board[2] - '0');
    print(" | ");
    if (xox_board[3] == 'X' || xox_board[3] == 'O') print_char(xox_board[3]); else print_int(xox_board[3] - '0');
    print(" \n");
    print("---+---+---\n");
    print(" ");
    if (xox_board[4] == 'X' || xox_board[4] == 'O') print_char(xox_board[4]); else print_int(xox_board[4] - '0');
    print(" | ");
    if (xox_board[5] == 'X' || xox_board[5] == 'O') print_char(xox_board[5]); else print_int(xox_board[5] - '0');
    print(" | ");
    if (xox_board[6] == 'X' || xox_board[6] == 'O') print_char(xox_board[6]); else print_int(xox_board[6] - '0');
    print(" \n");
    print("---+---+---\n");
    print(" ");
    if (xox_board[7] == 'X' || xox_board[7] == 'O') print_char(xox_board[7]); else print_int(xox_board[7] - '0');
    print(" | ");
    if (xox_board[8] == 'X' || xox_board[8] == 'O') print_char(xox_board[8]); else print_int(xox_board[8] - '0');
    print(" | ");
    if (xox_board[9] == 'X' || xox_board[9] == 'O') print_char(xox_board[9]); else print_int(xox_board[9] - '0');
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

int findMove(char mark) {
    int i;
    for (i = 1; i <= 9; i++) {
        if (xox_board[i] != 'X' && xox_board[i] != 'O') {
            char temp = xox_board[i];
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

    
    move = findMove('O');
    if (move) return move;

    
    move = findMove('X');
    if (move) return move;

    
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
        xox_board[i] = '0' + i;
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
            print("), enter position (1-9): \n");
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
    print("Choose mode: \n");
    get_line(input, sizeof(input));
    mode = simple_atoi(input);

    if (mode == 1) {
        getAutoWord(secret);
    } else if (mode == 2) {
        print("Player 1, enter the secret word: \n");
        get_line(input, sizeof(input));
        for (i = 0; input[i] != '\0' && input[i] != '\n'; i++)
            secret[i] = toUpper(input[i]);
        secret[i] = '\0';
        print("\n\n\n\n\n"); 
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

        print("Enter a letter: \n");
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

void xox_entry() {
    play_xox();
}

void hangman_entry() {
    play_hangman();
}