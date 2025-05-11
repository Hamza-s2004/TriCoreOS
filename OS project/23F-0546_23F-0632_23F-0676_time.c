#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

void showTime() {
    time_t t;
    struct tm *tm_info;

    time(&t);                    // Get current time
    tm_info = localtime(&t);     // Convert to local time

    printf("Current time: %02d:%02d:%02d\n",
           tm_info->tm_hour,
           tm_info->tm_min,
           tm_info->tm_sec);
}

int main() {
    showTime();  // Call the function to display the time
    sleep(2);
    return 0;
}

