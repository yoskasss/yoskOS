#include <stdio.h>

#define MAX_LIVES 6
#define MAX_WORD 20
#define WORD_COUNT 6

/* lowercase -> uppercase */
char toUpper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

void drawHangman(int lives) {
    printf("\n");
    if (lives <= 5) printf(" O \n"); else printf("\n");
    if (lives <= 4) printf("/"); else printf(" ");
    if (lives <= 3) printf("|"); else printf(" ");
    if (lives <= 2) printf("\\\n"); else printf("\n");
    if (lives <= 1) printf("/ "); else printf("  ");
    if (lives <= 0) printf("\\\n"); else printf("\n");
}

/* simple automatic word selector */
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

int main() {
    char secret[MAX_WORD];
    char guessed[MAX_WORD];
    char used[26];
    int usedCount = 0;

    int lives = MAX_LIVES;
    int mode;
    int i, correct, win = 0;
    char guess;

    printf("=== HANGMAN GAME ===\n");
    printf("1. Single Player\n");
    printf("2. Two Player\n");
    printf("Choose mode: ");
    scanf("%d", &mode);

    if (mode == 1) {
        getAutoWord(secret);
    } else if (mode == 2) {
        printf("Player 1, enter the secret word: ");
        scanf("%s", secret);

        for (i = 0; secret[i] != '\0'; i++)
            secret[i] = toUpper(secret[i]);

        printf("\n\n\n\n\n"); /* hide word */
    } else {
        printf("Invalid mode!\n");
        return 0;
    }

    for (i = 0; secret[i] != '\0'; i++)
        guessed[i] = '_';
    guessed[i] = '\0';

    printf("Player 2, start guessing!\n");

    while (lives > 0 && !win) {
        printf("\nWord: %s\n", guessed);
        printf("Lives left: %d\n", lives);
        drawHangman(lives);

        printf("Used letters: ");
        for (i = 0; i < usedCount; i++)
            printf("%c ", used[i]);
        printf("\n");

        printf("Enter a letter: ");
        scanf(" %c", &guess);

        guess = toUpper(guess);

        for (i = 0; i < usedCount; i++) {
            if (used[i] == guess) {
                printf("Letter already used!\n");
                guess = 0;
                break;
            }
        }
        if (guess == 0)
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
            printf("Wrong guess!\n");
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
        printf("\n🎉 YOU WIN!\n");
    } else {
        drawHangman(lives);
        printf("\n💀 GAME OVER!\n");
    }

    printf("The word was: %s\n", secret);
    return 0;
}

