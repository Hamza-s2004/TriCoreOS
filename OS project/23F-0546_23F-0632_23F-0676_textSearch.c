#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char filename[100], keyword[100];
    printf("Enter file name: ");
    scanf("%s", filename);
    printf("Enter keyword to search: ");
    scanf("%s", keyword);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("File error");
        sleep(1);
        return 1;
    }

    char line[512];
    int lineNum = 0, found = 0;

    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        if (strstr(line, keyword)) {
            printf("Line %d: %s", lineNum, line);
            found = 1;
        }
    }

    if (!found) {
        printf("No matches found for \"%s\".\n", keyword);
    }

    fclose(file);
    printf("Search completed.\n");
    sleep(2); // Simulate some delay
    return 0;
}
