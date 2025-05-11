#include<stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void interrupt_self()
{
    printf("Interrupt requested. Terminating create task...\n");
    kill(getpid(),SIGTERM);
}

void createFile() {
    char filename[100];
    printf("Enter the name of the file to create: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "w");
    if (file== NULL) {
        printf("Error creating file %s\n", filename);
        return;
    }
    printf("File %s created successfully.\n", filename);
    fclose(file);
    sleep(1);
}

int main(){
    printf("=== File Creation Task ===\n");
    printf("1. Create File\n2. Exit\nChoose: ");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        createFile();
        sleep(2);
        break;
    case 2:
        interrupt_self();
        break;
    default:
        printf("Invalid choice. Exiting...\n");
        break;
    }
}