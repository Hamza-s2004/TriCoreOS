#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
void movingFile()
{
    char source[100], destination[100];
    printf("Enter the source file name: ");
    scanf("%s", source);
    printf("Enter the destination file name: ");
    scanf("%s", destination);
    if (source == NULL || destination == NULL) {
        printf("Error: Invalid file names.\n");
        return;
    }
    else if (source == destination) {
        printf("Error: Source and destination file names are the same.\n");
        return;
    }
    else if (rename(source, destination) == 0) {
        printf("File moved successfully from %s to %s\n", source, destination);
    } 
    else {
        printf("Error moving file from %s to %s\n", source, destination);
    }
}
void interrupt_self()
{
    printf("Interrupt requested. Terminating move task...\n");
    kill(getpid(),SIGTERM);
}
int main(){
    printf("=== File Move Task ===\n");
    printf("1. Move File\n2. Interrupt Task\nChoose: ");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        movingFile();
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