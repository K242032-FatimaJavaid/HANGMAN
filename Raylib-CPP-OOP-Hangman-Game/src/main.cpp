#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 5
#define MAX_MISTAKES 6

const char *wordList[MAX_WORDS] = {
    "RAYLIB", "HANGMAN", "COMPUTER", "PROGRAM", "GAMING"
};

typedef struct {
    char word[20];
    char guessed[20];
    int mistakes;
    int lettersGuessed[26]; // A-Z
} GameState;

void InitGame(GameState *game) {
    srand(time(NULL));
    int index = rand() % MAX_WORDS;
    strcpy(game->word, wordList[index]);
    memset(game->guessed, '_', strlen(game->word));
    game->guessed[strlen(game->word)] = '\0';
    game->mistakes = 0;
    memset(game->lettersGuessed, 0, sizeof(game->lettersGuessed));
}

bool IsGameWon(GameState *game) {
    return strcmp(game->word, game->guessed) == 0;
}

bool IsGameOver(GameState *game) {
    return game->mistakes >= MAX_MISTAKES;
}

void DrawHangman(int mistakes) {
    // Base frame
    DrawLine(50, 400, 200, 400, BLACK);      // base
    DrawLine(125, 400, 125, 100, BLACK);     // pole
    DrawLine(125, 100, 250, 100, BLACK);     // top
    DrawLine(250, 100, 250, 150, BLACK);     // rope

    if (mistakes > 0) DrawCircle(250, 170, 20, BLACK);               // head
    if (mistakes > 1) DrawLine(250, 190, 250, 250, BLACK);           // body
    if (mistakes > 2) DrawLine(250, 200, 230, 230, BLACK);           // left arm
    if (mistakes > 3) DrawLine(250, 200, 270, 230, BLACK);           // right arm
    if (mistakes > 4) DrawLine(250, 250, 230, 290, BLACK);           // left leg
    if (mistakes > 5) DrawLine(250, 250, 270, 290, BLACK);           // right leg
}

void DrawUI(GameState *game) {
    DrawText("Hangman Game (Raylib)", 400, 20, 24, DARKGRAY);
    DrawText("Guess the word:", 400, 70, 20, BLACK);
    DrawText(game->guessed, 400, 100, 40, MAROON);

    char missed[100] = "Missed letters: ";
    int len = strlen(missed);
    for (int i = 0; i < 26; i++) {
        if (game->lettersGuessed[i] && strchr(game->word, 'A' + i) == NULL) {
            missed[len++] = 'A' + i;
            missed[len++] = ' ';
        }
    }
    missed[len] = '\0';
    DrawText(missed, 400, 160, 20, RED);
}



int main() {
    InitWindow(800, 450, "Hangman Game - Raylib");
    SetTargetFPS(60);

    GameState game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        // Handle key press
        for (int key = KEY_A; key <= KEY_Z; key++) {
            if (IsKeyPressed(key)) {
                char letter = 'A' + (key - KEY_A);
                int index = letter - 'A';

                if (!game.lettersGuessed[index]) {
                    game.lettersGuessed[index] = 1;
                    bool correct = false;
                    for (int i = 0; i < strlen(game.word); i++) {
                        if (game.word[i] == letter) {
                            game.guessed[i] = letter;
                            correct = true;
                        }
                    }
                    if (!correct) {
                        game.mistakes++;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawHangman(game.mistakes);
        DrawUI(&game);

        if (IsGameWon(&game)) {
            DrawText("You Win! Press R to Restart", 400, 300, 20, GREEN);
            if (IsKeyPressed(KEY_R)) InitGame(&game);
        } else if (IsGameOver(&game)) {
            DrawText("Game Over! Press R to Restart", 400, 300, 20, RED);
            DrawText(TextFormat("Word was: %s", game.word), 400, 330, 20, GRAY);
            if (IsKeyPressed(KEY_R)) InitGame(&game);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}