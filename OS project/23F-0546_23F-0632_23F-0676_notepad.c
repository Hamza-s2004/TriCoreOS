# include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
typedef struct note 
{
    FILE* fp;
    char* name;
    void (*write)(struct note*);
    void (*close)(struct note*);
}note;

//now we will write the function to write the note with auto save feature

void write_note(note* n)
{
    char buffer[1000];
    printf("Enter your note (type 'exit' to exit):\n");
    while (1) 
    {
        fgets(buffer, sizeof(buffer), stdin);
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
        fputs(buffer, n->fp);
        fflush(n->fp); // Ensure all data is written to the file
    }
    printf("Note written successfully.\n");
    sleep(2); // Simulate some delay
    
}

void close_note(note* n)
{
    fclose(n->fp);
    printf("Note saved to %s\n", n->name);
}


void init_note(note* n, const char* name)
{
    n->name = malloc(strlen(name) + 1);
    strcpy(n->name, name);
    n->fp = fopen(name, "w+");
    if (n->fp == NULL) {
        perror("Failed to open file");
        exit(1);
    }
    n->write = write_note;
    n->close = close_note;
    printf("Note initialized with name: %s\n", n->name);
}


void interrupt_self()
{
    printf("Interrupt requested. Terminating note task...\n");
    kill(getpid(), SIGTERM);
}
int main() {
    note n;
    init_note(&n, "note.txt");
    
    printf("1. Write Note\n2. Interrupt Task\nChoose: ");
    int choice;
    scanf("%d", &choice);
    getchar(); // Consume newline character left by scanf
    switch (choice) {
    case 1:
        n.write(&n);
        break;
    case 2:
        interrupt_self();
        break;
    default:
        printf("Invalid choice. Please select 1 or 2.\n");
        break;
    }
    return 0;
}
