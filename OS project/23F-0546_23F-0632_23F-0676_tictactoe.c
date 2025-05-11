#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
char board[3][3];
char currentPlayer;

void initializeBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

void printBoard() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

int isBoardFull() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') return 0; // There is still space
        }
    }
    return 1; // Board is full
}

int checkWin() {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') || 
            (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')) {
            return 1;
        }
    }
    // Check diagonals
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')) {
        return 1;
    }
    return 0;
}

int validMove(int row, int col) {
    return (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ');
}

void playerMove() {
    int row, col;
    printf("Player %c, enter row (0-2) and column (0-2): ", currentPlayer);
    scanf("%d %d", &row, &col);
    while (!validMove(row, col)) {
        printf("Invalid move! Try again: ");
        scanf("%d %d", &row, &col);
    }
    board[row][col] = currentPlayer;
}
void interrupt_self() {
    printf("Interrupt requested. Terminating Tic Tac Toe...\n");
    sleep(1);
    kill(getpid(), SIGTERM);
}

int main() {
    printf("Welcome to Tic Tac Toe!\n");
    printf("Press 1 to start the game or 2 to interrupt game: ");
    int choice;
    scanf("%d", &choice);
    if (choice == 2) {
        interrupt_self();
    }
    int gameOver = 0;
    currentPlayer = 'X';

    initializeBoard();

    while (!gameOver) {
        printBoard();
        playerMove();

        if (checkWin()) {
            printBoard();
            printf("Player %c wins!\n", currentPlayer);
            gameOver = 1;
        } else if (isBoardFull()) {
            printBoard();
            printf("It's a draw!\n");
            gameOver = 1;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; // Switch player
    }
    sleep(2);
    return 0;
}
