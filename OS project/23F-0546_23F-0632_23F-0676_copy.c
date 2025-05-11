#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void interrupt_self() {
    printf("Interrupt requested. Terminating copy task...\n");
    kill(getpid(), SIGTERM);
}

int main() {
    int choice;
    printf("=== Copy Task ===\n");
    printf("1. Perform Copy\n");
    printf("2. Interrupt Task\n");
    printf("Choose: ");
    scanf("%d", &choice);

    if (choice == 2) {
        interrupt_self();
        return 0;
    }

    char src[100], dst[100];
    printf("Source file: ");
    scanf("%s", src);
    printf("Destination file: ");
    scanf("%s", dst);

    int fd1 = open(src, O_RDONLY);
    if (fd1 < 0) {
        printf("Can't open source file\n");
        return 1;
    }
    int fd2 = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) {
        printf("Can't open destination file\n");
        close(fd1);
        return 1;
    }
    char buf[100];
    int n;
    while ((n = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, n);
    }
    printf("Copy complete\n");
    close(fd1);
    close(fd2);
    sleep(2); // Simulate some delay
    return 0;
}
