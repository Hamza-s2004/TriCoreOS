#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void interrupt_self() {
    printf("Interrupt requested. Terminating calculator task...\n");
    kill(getpid(), SIGTERM);
}

int main() {
    int choice;
    printf("=== Calculator Task ===\n");
    printf("1. Perform Calculation\n");
    printf("2. Interrupt Task (Simulate)\n");
    printf("Choose: ");
    scanf("%d", &choice);

    if (choice == 2) {
        interrupt_self(); // simulates manual interrupt
        return 0;
    }

    int a, b, op;
    printf("Enter two numbers: ");
    scanf("%d %d", &a, &b);
    printf("1. Add\n2. Sub\n3. Mul\n4. Div\nChoose operation: ");
    scanf("%d", &op);
    switch (op) {
        case 1: printf("Result: %d\n", a + b); break;
        case 2: printf("Result: %d\n", a - b); break;
        case 3: printf("Result: %d\n", a * b); break;
        case 4: b != 0 ? printf("Result: %.2f\n", (float)a / b) : printf("Div by zero!\n"); break;
        default: printf("Invalid\n");
    }
    sleep(2);
    return 0;
}
