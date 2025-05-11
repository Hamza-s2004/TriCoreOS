#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Launching Process Monitor...\n");
    printf("Press Ctrl+C to exit.\n");
    sleep(1);
    system("top");
    sleep(2);
    return 0;
}
