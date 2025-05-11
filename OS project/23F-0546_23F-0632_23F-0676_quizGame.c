#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    int correct = 0, total = 5;
    int a, b, answer, userAnswer, op;

    srand(time(NULL)); // Seed random number generator

    printf("Welcome to the Math Quiz Game!\n\n");

    for (int i = 0; i < total; i++) {
        a = rand() % 50 + 1;   // 1 to 50
        b = rand() % 50 + 1;
        op = rand() % 3;       // 0: +, 1: -, 2: *

        switch (op) {
            case 0:
                answer = a + b;
                printf("Q%d: %d + %d = ", i + 1, a, b);
                break;
            case 1:
                answer = a - b;
                printf("Q%d: %d - %d = ", i + 1, a, b);
                break;
            case 2:
                answer = a * b;
                printf("Q%d: %d * %d = ", i + 1, a, b);
                break;
        }

        scanf("%d", &userAnswer);
        if (userAnswer == answer) {
            printf("Correct!\n\n");
            correct++;
        } else {
            printf("Wrong! Correct answer: %d\n\n", answer);
        }
    }

    printf("You got %d out of %d correct!\n", correct, total);
    sleep(2);
    return 0;
}
