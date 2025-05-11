#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Listing Directory Contents...\n");
    system("ls -l");
    sleep(3); // Simulate some delay
    return 0;
}
