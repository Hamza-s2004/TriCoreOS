#include<stdio.h>
#include<signal.h>
#include <stdlib.h>
#include <unistd.h>

void interrupt_self()
{
    printf("Interrupt requested. Terminating delete task...\n");
    kill(getpid(),SIGTERM);
}

void deletingFile()
{
    char filename[100];
    printf("Enter the name of the file to delete: ");
    scanf("%s", filename);

    if (remove(filename) == 0) {
        printf("File %s deleted successfully.\n", filename);
    } else {
        printf("Error deleting file %s\n", filename);
    }
}

int main(){
    printf("=== File Deletion Task ===\n");
    printf("1. Delete File\n2. Exit\nChoose: ");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        deletingFile();
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