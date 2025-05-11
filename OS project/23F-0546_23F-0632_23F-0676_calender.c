#include <stdio.h>
#include<unistd.h>
#include <signal.h>

int getDayOfWeek(int day, int month, int year) {
    if (month < 3) {
        month += 12;
        year--;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (day + (13*(month + 1))/5 + k + k/4 + j/4 + 5*j) % 7;
    return (h + 6) % 7; // Make Sunday=0
}

int getDaysInMonth(int month, int year) {
    switch (month) {
        case 1: return 31;
        case 2: return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
        case 3: return 31;
        case 4: return 30;
        case 5: return 31;
        case 6: return 30;
        case 7: return 31;
        case 8: return 31;
        case 9: return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;
        default: return 0;
    }
}

void printCalendar(int month, int year) {
    const char *months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    printf("\n   %s %d\n", months[month - 1], year);
    printf("Su Mo Tu We Th Fr Sa\n");

    int startDay = getDayOfWeek(1, month, year);
    int days = getDaysInMonth(month, year);

    for (int i = 0; i < startDay; i++) {
        printf("   ");
    }

    for (int day = 1; day <= days; day++) {
        printf("%2d ", day);
        if ((startDay + day) % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
void interrupt_self() {
    printf("Interrupt requested. Terminating calendar task...\n");
    kill(getpid(), SIGTERM);
}

int main() {
    printf("=== Calendar Program ===\n");
    printf("1. Show Calendar\n2. Exit\nChoose: ");
    int choice;
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        int month, year;
        printf("Enter month (1-12): ");
        scanf("%d", &month);
        printf("Enter year: ");
        scanf("%d", &year);
        printCalendar(month, year);
        sleep(2);
        break;
    case 2:
        printf("Exiting the program...\n");
        return 0;
    default:
        printf("Invalid choice. Please try again.\n");
        break;
    }
}
