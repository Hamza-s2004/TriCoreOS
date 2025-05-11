#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include<signal.h>
#define ROWS 5
#define COLS 5
#define MINES 5

typedef struct Minesweeper {
    char grid[ROWS][COLS];
    char revealed[ROWS][COLS];

    void (*init)(struct Minesweeper *);
    void (*display)(struct Minesweeper *);
    int  (*reveal)(struct Minesweeper *, int, int);
    void (*playGame)(struct Minesweeper *);
} Minesweeper;

void init_game(Minesweeper *game) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            game->grid[i][j] = '0';
            game->revealed[i][j] = '-';
        }

    int placed = 0;
    while (placed < MINES) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (game->grid[r][c] == 'M') continue;
        game->grid[r][c] = 'M';
        placed++;

        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                int x = r + i, y = c + j;
                if (x >= 0 && x < ROWS && y >= 0 && y < COLS && game->grid[x][y] != 'M') {
                    game->grid[x][y]++;
                }
            }
    }
}

void display_grid(Minesweeper *game) {
    printf("\nCurrent Grid:\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", game->revealed[i][j]);
        }
        printf("\n");
    }
}

int reveal_cell(Minesweeper *game, int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS || game->revealed[row][col] != '-') return 0;

    game->revealed[row][col] = game->grid[row][col];
    if (game->grid[row][col] == 'M') return 1;

    if (game->grid[row][col] == '0') {
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
                reveal_cell(game, row + i, col + j);
    }

    return 0;
}

void play_game(Minesweeper *game) {
    // Set function pointers
    game->init = init_game;
    game->display = display_grid;
    game->reveal = reveal_cell;

    srand(time(0));           // Random seed here
    game->init(game);         // Initialize the board

    int game_over = 0;
    while (!game_over) {
        game->display(game);
        int r, c;
        printf("Enter row and column (0 to %d): ", ROWS - 1);
        scanf("%d %d", &r, &c);

        game_over = game->reveal(game, r, c);
        if (game_over) {
            printf("\nBoom! You hit a mine.\n");
            // Reveal entire grid
            for (int i = 0; i < ROWS; i++)
                for (int j = 0; j < COLS; j++)
                    game->revealed[i][j] = game->grid[i][j];

            game->display(game);
        }
    }
}
void interrupt_self() {
    printf("Interrupt requested. Terminating game...\n");
    kill(getpid(), SIGTERM);
}

int main()
{
    Minesweeper game;
    game.playGame = play_game;

    printf("=== Minesweeper Game ===\n");
    printf("1. Start Game\n2. Interrupt Task\nChoose: ");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            game.playGame(&game);
            sleep(2);
            break;
        case 2:
            interrupt_self();
            break;
        default:
            printf("Invalid choice. Exiting...\n");
            break;
    }
    return 0;

}
