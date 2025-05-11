#include<stdio.h>
#include<sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include<time.h>

//we are going to write code for file state

void fileState()
{
    char filename[100];
    struct stat fileStat;
    printf("Enter the name of the file: ");
    scanf("%s", filename);
    if (stat(filename, &fileStat) < 0) {
        printf("Error getting file state for %s\n", filename);
        return;
    }
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", fileStat.st_size);
    printf("Last accessed: %s", ctime(&fileStat.st_atime));
    printf("Last modified: %s", ctime(&fileStat.st_mtime));
    printf("Last changed: %s", ctime(&fileStat.st_ctime));
    printf("Permissions: %o\n", fileStat.st_mode & 0777);
    printf("Number of links: %ld\n", fileStat.st_nlink);
    printf("Owner ID: %d\n", fileStat.st_uid);
    printf("Group ID: %d\n", fileStat.st_gid);
    printf("Device ID: %ld\n", fileStat.st_dev);
    printf("Inode number: %ld\n", fileStat.st_ino);
}

int main() {
    fileState();
    sleep(2);
    return 0;
}