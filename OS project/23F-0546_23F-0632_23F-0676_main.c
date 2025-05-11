
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "23F-0546_23F-0632_23F-0676_resouce.c"
#include <pthread.h>

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Semaphore;

void initSemaphore(Semaphore* s, int initial) {
    s->value = initial;
    pthread_mutex_init(&s->mutex, NULL);
    pthread_cond_init(&s->cond, NULL);
}

void waitSemaphore(Semaphore* s) {
    pthread_mutex_lock(&s->mutex);
    while (s->value <= 0)
        pthread_cond_wait(&s->cond, &s->mutex);
    s->value--;
    pthread_mutex_unlock(&s->mutex);
}

void signalSemaphore(Semaphore* s) {
    pthread_mutex_lock(&s->mutex);
    s->value++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
}


typedef struct PCB {
    int pid;
    char name[32];
    char state[16];
    int ram, hdd, cores;
    int priority;
    int execTime;
    char mode[8]; // Added for kernel/user mode
} PCB;

typedef struct Queue {
    PCB** items;
    int front, rear, capacity;
} Queue;

PCB* processTable = NULL;
int processCount = 0;
int processCapacity = 0;

Queue highQueue, medQueue, lowQueue;

Semaphore queueMutex;     // For critical section (queue operations)


// Function to add a PCB dynamically
void addPCB(int pid, const char* name, int ram, int hdd, int cores, const char* state, int priority, int execTime) {
    if (processCount >= processCapacity) {
        processCapacity = processCapacity == 0 ? 10 : processCapacity * 2;
        processTable = (PCB*)realloc(processTable, processCapacity * sizeof(PCB));
    }
    PCB* p = &processTable[processCount++];
    p->pid = pid;
    strcpy(p->name, name);
    strcpy(p->state, state);
    p->ram = ram;
    p->hdd = hdd;
    p->cores = cores;
    p->priority = priority;
    p->execTime = execTime;
    strcpy(p->mode, "USER");  // Default mode
}

void updatePCBState(int pid, const char* state) {
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].pid == pid) {
            strcpy(processTable[i].state, state);
            break;
        }
    }
}

void switchToKernelMode(PCB* p) {
    if (strcmp(p->state, "TERMINATED") == 0) {
        printf("Cannot switch mode: process %d is already TERMINATED.\n", p->pid);
        return;
    }
    if (strcmp(p->mode, "KERNEL") != 0) {
        printf("Switching process %d (%s) to KERNEL mode.\n", p->pid, p->name);
        strcpy(p->mode, "KERNEL");
    } else {
        printf("Already in KERNEL mode.\n");
    }
}

void switchToUserMode(PCB* p) {
    if (strcmp(p->state, "TERMINATED") == 0) {
        printf("Cannot switch mode: process %d is already TERMINATED.\n", p->pid);
        return;
    }
    if (strcmp(p->mode, "USER") != 0) {
        printf("Switching process %d (%s) to USER mode.\n", p->pid, p->name);
        strcpy(p->mode, "USER");
    } else {
        printf("Already in USER mode.\n");
    }
}

void printPCBTable() {
    printf("\n--- PCB Table ---\n");
    printf("%-5s %-12s %-10s %-5s %-5s %-5s %-8s %-8s %-7s\n", "PID", "Name", "State", "RAM", "HDD", "CPU", "Priority", "ExecTime", "Mode");
    for (int i = 0; i < processCount; i++) {
        PCB* p = &processTable[i];
        printf("%-5d %-12s %-10s %-5d %-5d %-5d %-8d %-8d %-7s\n", p->pid, p->name, p->state, p->ram, p->hdd, p->cores, p->priority, p->execTime, p->mode);
    }
}

// Add this function near other utility functions
void terminateProcess(ResourceManager* r) {
    int pidt;
    printf("Enter PID to terminate: ");
    scanf("%d", &pidt);

    for (int i = 0; i < processCount; i++) {
        PCB* p = &processTable[i];
        if (p->pid == pidt) 
        {
            if (strcmp(p->mode, "KERNEL") != 0) {
                printf("Cannot terminate. Process %d is not in KERNEL mode.\n", pidt);
                return;
            }
            if (strcmp(p->state, "READY") == 0) {
                strcpy(p->state, "TERMINATED");
                printf("Process %d (%s) is terminated.\n", pidt, p->name);
                r->deallocate(r, p->ram, p->hdd, p->cores);
                return;
            }

            if (strcmp(p->state, "TERMINATED") == 0) {
                printf("Process already terminated.\n");
                return;
            }
            

            if (kill(pidt, SIGKILL) == 0) {
                printf("Process %d (%s) terminated successfully.\n", pidt, p->name);
                strcpy(p->state, "TERMINATED");
                r->deallocate(r, p->ram, p->hdd, p->cores);
            } else {
                printf("kill failed");
            }
            sleep(2);
            return;
        }
    }
    printf("PID not found!\n");
}



// Function to initialize a queue
void initQueue(Queue* q, int initialCapacity) {
    q->items = (PCB**)malloc(initialCapacity * sizeof(PCB*));
    if (q->items == NULL) {
        printf("Memory allocation failed for queue items.\n");
        exit(1);
    }
    q->front = q->rear = 0;
    q->capacity = initialCapacity;
}

void resizeQueue(Queue* q) {
    int newCapacity = q->capacity * 2;
    PCB** newItems = (PCB**)malloc(newCapacity * sizeof(PCB*));
    if (newItems == NULL) {
        printf("Memory allocation failed for resized queue items.\n");
        exit(1);
    }

    // Rearrange elements in the correct order
    int count = (q->rear - q->front + q->capacity) % q->capacity;
    for (int i = 0; i < count; i++) {
        newItems[i] = q->items[(q->front + i) % q->capacity];
    }

    free(q->items);
    q->items = newItems;
    q->front = 0;
    q->rear = count;
    q->capacity = newCapacity;
}

// Function to enqueue a PCB into a queue
void enqueue(Queue* q, PCB* process) {
    waitSemaphore(&queueMutex);

    // Check if the queue is full
    if ((q->rear + 1) % q->capacity == q->front) {
        resizeQueue(q); // Resize the queue if full
        
    }

    // Add the process to the queue
    q->items[q->rear] = process;
    q->rear = (q->rear + 1) % q->capacity; // Move rear pointer circularly

    signalSemaphore(&queueMutex);
}
void enqueueByPriority(PCB* p) {
    if (p->priority == 0) {
        enqueue(&highQueue, p); // High-priority queue
    } else if (p->priority == 1) {
        enqueue(&medQueue, p); // Medium-priority queue
    } else {
        enqueue(&lowQueue, p); // Low-priority queue
    }
}

// Function to dequeue a PCB from a queue
PCB* dequeue(Queue* q) {
    waitSemaphore(&queueMutex);
    if (q->front == q->rear) {
        signalSemaphore(&queueMutex);
        return NULL;
    }
    PCB* p = q->items[q->front++];
    signalSemaphore(&queueMutex);
    return p;
}

void sortLowQueue() {
    int count = (lowQueue.rear - lowQueue.front + lowQueue.capacity) % lowQueue.capacity;
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int idx1 = (lowQueue.front + j) % lowQueue.capacity;
            int idx2 = (lowQueue.front + j + 1) % lowQueue.capacity;
            if (lowQueue.items[idx1]->execTime > lowQueue.items[idx2]->execTime) {
                PCB* temp = lowQueue.items[idx1];
                lowQueue.items[idx1] = lowQueue.items[idx2];
                lowQueue.items[idx2] = temp;
            }
        }
    }
}

void forkAndExec(PCB* p, ResourceManager* r) {
    int action;
    printf("\nPrepare to run \"%s\". Choose action:\n", p->name);
    printf("1. Cancel\n2. Minimize\n3. Run Normally\nChoice: ");
    scanf("%d", &action);

    if (action == 1) {
        printf("Task \"%s\" canceled before execution.\n", p->name);
        updatePCBState(p->pid, "TERMINATED");
        r->deallocate(r, p->ram, p->hdd, p->cores);
        return;
    }
    pid_t pid = fork();
    if (pid == 0) {
        char execPath[64];
        sprintf(execPath, "./%s", p->name);
        //execl("/usr/bin/xterm", "xterm", "-fa", "Monospace", "-fs", "10", "-e", execPath, NULL);
        execlp("xterm", "xterm", "-fa", "Monospace", "-fs", "10", "-e", execPath, NULL);
        printf("Exec failed");
        exit(1);
    } else {
        p->pid = pid;

        if (action == 2) {
            kill(pid, SIGSTOP);
            strcpy(p->state, "PAUSED");
            printf("Task \"%s\" minimized (PID: %d).\n", p->name, pid);
        } else {
            strcpy(p->state, "RUNNING");
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                updatePCBState(pid, "TERMINATED");
                r->deallocate(r, p->ram, p->hdd, p->cores);
            }
        }
    }
}
void initializeSystem() {
    processTable = NULL;
    processCount = 0;
    processCapacity = 0;

    initQueue(&highQueue, 10);
    initQueue(&medQueue, 10);
    initQueue(&lowQueue, 10);
}



// Function to clean up dynamically allocated memory
void cleanupSystem() {
    free(processTable);
    free(highQueue.items);
    free(medQueue.items);
    free(lowQueue.items);
}


void scheduleTasks(ResourceManager* r) {
    int running = 0;
    while (highQueue.front != highQueue.rear && running < r->totalCores) {
        PCB* p = dequeue(&highQueue);
        if (strcmp(p->state, "TERMINATED") == 0) {
            continue;
        }
        forkAndExec(p, r);
        running++;
    }
    running=0;
    while (medQueue.front != medQueue.rear && running < r->totalCores) {
        PCB* p = dequeue(&medQueue);
        if (strcmp(p->state, "TERMINATED") == 0) {
            continue;
        }
        forkAndExec(p, r);
        running++;
    }
    running = 0;
    sortLowQueue();
    for (int i = 0; i < lowQueue.rear && running < r->totalCores; i++) {
        if (strcmp(lowQueue.items[i]->state, "TERMINATED") == 0) {
            continue;
        }
        forkAndExec(lowQueue.items[i], r);
        running++;
    }
    lowQueue.rear = 0;
}

void* schedulerLoop(void* arg) {
    ResourceManager* r = (ResourceManager*)arg;
    scheduleTasks(r);      // Dispatch all ready processes once
    return NULL;
}

void resumeMinimized(ResourceManager* r) {
    int pid;
    printf("Enter PID to resume: ");
    scanf("%d", &pid);

    PCB* p = NULL;
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].pid == pid) {
            p = &processTable[i];
            break;
        }
    }

    if (p == NULL) {
        printf("PID %d not found!\n", pid);
        return;
    }

    if (strcmp(p->state, "PAUSED") != 0) {
        printf("Process %d is not in PAUSED state.\n", pid);
        return;
    }

    if (kill(pid, SIGCONT) == 0) {
        updatePCBState(pid, "RUNNING");
        printf("Task %d resumed.\n", pid);
        
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            updatePCBState(pid, "TERMINATED");
            r->deallocate(r, p->ram, p->hdd, p->cores);
        }
    } else {
        printf("Failed to resume process\n");
    }
}

void bootingScreen() {
    system("clear"); // Clear the screen
    printf("\n\n\n\n\n\n");
    printf("\t\t\t========================================\n");
    printf("\t\t\t        Welcome to H3OS\n");
    printf("\t\t\t  Powered by Hamza, Hassan, and Haider\n");
    printf("\t\t\t========================================\n");
    sleep(3); // Pause for 3 seconds to simulate booting
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <RAM in GB> <HDD in GB> <Cores>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    int ram = atoi(argv[1]);
    int hdd = atoi(argv[2]);
    int cores = atoi(argv[3]);

    // checking if the ram, hdd and cores are correctly inputted or not
    if (ram <= 0 || hdd <= 0 || cores <= 0) {
        printf("Error: All values must be positive integers.\n");
        return 1;
    }
    ram *= 1024; // Convert GB to MB
    hdd *= 1024; // Convert GB to MB
    bootingScreen();
    system("clear"); // Clear the screen
    initializeSystem();
    ResourceManager r = { .totalRAM = ram, .usedRAM = 0,
                          .totalHDD = hdd, .usedHDD = 0,
                          .totalCores = cores, .usedCores = 0,
                          .allocate = allocate_resources,
                          .deallocate = deallocate_resources,
                          .showStatus = show_resource_status };

    int choice;
    char compileCmd[256];
    initSemaphore(&queueMutex, 1);                  // Mutex-style semaphore

    sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_time.c -o time.out");
    if (system(compileCmd) != 0) return 0;
    if (r.allocate(&r, 30, 5, 1)) {
        addPCB(0, "time.out", 30, 5, 1, "READY", 0, 2);
        enqueueByPriority(&processTable[processCount - 1]);
    }
    else {
        printf("Not enough resources for Time task.\n");
    }
    sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_calender.c -o calendar.out");
    if (system(compileCmd) != 0) return 0;
    if (r.allocate(&r, 40, 5, 1)) {
        addPCB(1, "calendar.out", 40, 5, 1, "READY", 0, 2);
        enqueueByPriority(&processTable[processCount - 1]);
    }
    else {
        printf("Not enough resources for Calendar task.\n");
    }
    pthread_t schedThread;
    if (pthread_create(&schedThread, NULL, schedulerLoop, &r) != 0) {
        printf("Failed to create scheduler thread");
        return 1;
    }
    pthread_join(schedThread, NULL);
    int counter=2;
    while (1) 
    {
        printf("\n=== Task Menu ===\n");
        printf("1. Calculator (RAM:50, HDD:10, Priority:2)\n");
        printf("2. File Copy  (RAM:150, HDD:300, Priority:1)\n");
        printf("3. Create file (RAM:30, HDD:10, Priority:0)\n");
        printf("4. Move file (RAM:40, HDD:10, Priority:0)\n");
        printf("5. Write Note (RAM:50, HDD:20, Priority:0)\n");
        printf("6. Delete file (RAM:30, HDD:10, Priority:0)\n");
        printf("7. File info (RAM:20, HDD:10, Priority:0)\n");
        printf("8. Time (RAM:30, HDD:5, Priority:0)\n");
        printf("9. MineSweeper game (RAM:120, HDD:20, Priority:2)\n");
        printf("10. Calendar (RAM:40, HDD:5, Priority:0)\n");
        printf("11. Process Monitor (RAM:60, HDD:20, Priority:2)\n");
        printf("12. Math Quiz (RAM:60, HDD:20, Priority:1)\n");
        printf("13. Text Search (RAM:90, HDD:40, Priority:1)\n");
        printf("14. Tic-Tac-Toe (RAM:80, HDD:30, Priority:1)\n");
        printf("15. List Current Directory (RAM:40, HDD:10, Priority:0)\n");
        printf("16. Show Resource Status\n");
        printf("17. Show PCB Table\n");
        printf("18. Schedule Tasks\n");
        printf("19. Resume Minimized Task\n");
        printf("20. Switch Task Mode (USER/KERNEL)\n");
        printf("21. Terminate Process\n");
        printf("22. Exit\nChoose task: ");
        
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Calculator task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_calculator.c -o calculator.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 50, 10, 1)) {
                addPCB(counter, "calculator.out", 50, 10, 1, "READY", 2, 5);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Calculator task.\n");
            }
            break;
        case 2:
            printf("File Copy task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_copy.c -o copy.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 150, 300, 1)) {
                addPCB(counter, "copy.out", 150, 300, 1, "READY", 1, 3);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for File Copy task.\n");
            }
            break;
        case 3: 
            printf("Create file task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_createFile.c -o create.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 30, 10, 1)) {
                addPCB(counter, "create.out", 30, 10, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Create File task.\n");
            }
            break;
        case 4:
            printf("Move file task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_move.c -o move.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 40, 10, 1)) {
                addPCB(counter, "move.out", 40, 10, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Move File task.\n");
            }
            break;
        case 5:
            printf("Write Note task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_notepad.c -o note.out");
            if (system(compileCmd)!=0) return 0;
            if (r.allocate(&r, 50, 20, 1)) {
                addPCB(counter, "note.out", 50, 20, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Write Note task.\n");
            }
            break;
        case 6:
            printf("Delete file task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_deleteFile.c -o delete.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 30, 10, 1)) {
                addPCB(counter, "delete.out", 30, 10, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Delete File task.\n");
            }
            break;
        case 7:
            printf("File info task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_fileInfo.c -o fileinfo.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 20, 10, 1)) {
                addPCB(counter, "fileinfo.out", 20, 10, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for File Info task.\n");
            }
            break;
        case 8:
            printf("Time task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_time.c -o time.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 30, 5, 1)) {
                addPCB(counter, "time.out", 30, 5, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Time task.\n");
            }
            break;
        case 9:
            printf("Minesweeper task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_MineSweeper.c -o minesweeper.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 120, 20, 2)) {
                addPCB(counter, "minesweeper.out", 120, 20, 2, "READY", 2, 6);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Minesweeper task.\n");
            }
            break;
        case 10:
            printf("Calendar task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_calender.c -o calendar.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 40, 5, 1)) {
                addPCB(counter, "calendar.out", 40, 5, 1, "READY", 0, 2);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Calendar task.\n");
            }
            break;
        case 11:
            printf("Process Monitor task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_monitor.c -o monitor.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 60, 20, 1)) {
                addPCB(counter, "monitor.out", 60, 20, 1, "READY", 2, 4);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Process Monitor task.\n");
            }
            break;
        case 12:
            printf("Math Quiz selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_quizGame.c -o mathquiz.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 60, 20, 1)) {
                addPCB(counter, "mathquiz.out", 60, 20, 1, "READY", 1, 3);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Math Quiz task.\n");
            }
            break;
        case 13:
            printf("Text Search task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_textSearch.c -o textsearch.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 90, 40, 1)) {
                addPCB(counter, "textsearch.out", 90, 40, 1, "READY", 1, 3);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Text Search task.\n");
            }
            break;
        case 14:
            printf("Tic-Tac-Toe task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_tictactoe.c -o tictactoe.out");
            if (system(compileCmd) != 0) return 0;
            if (r.allocate(&r, 80, 30, 1)) {
                addPCB(counter, "tictactoe.out", 80, 30, 1, "READY", 1, 3);
                enqueueByPriority(&processTable[processCount - 1]);
                counter++;
            }
            else {
                printf("Not enough resources for Tic-Tac-Toe task.\n");
            }
            break;
        case 15:
            printf("List Current Directory task selected.\n");
            sprintf(compileCmd, "gcc 23F-0546_23F-0632_23F-0676_listDirectory.c -o listdir.out");
            if (system(compileCmd) != 0) return 0;  // Compilation failed
            if (r.allocate(&r, 40, 10, 1)) {       // Allocate resources
                addPCB(counter, "listdir.out", 40, 10, 1, "READY", 0, 2);  // Add PCB
                enqueueByPriority(&processTable[processCount - 1]);  // Add to queue
                counter++;
            }
            else {
                printf("Not enough resources for List Directory task.\n");
            }
            break;
        case 16:
            r.showStatus(&r);
            break;
        case 17:
            printPCBTable();
            sleep(2);
            break;
        case 18:
            scheduleTasks(&r);
            break;
        case 19:
            resumeMinimized(&r);
            break;
        
        case 20: 
            int pid, m;
            printf("Enter PID to switch mode: ");
            scanf("%d", &pid);
            PCB* found = NULL;
            for (int i = 0; i < processCount; i++) {
                if (processTable[i].pid == pid) {
                    found = &processTable[i];
                    break;
                }
            }
            if (!found) {
                printf("PID not found!\n");
                break;
            }
            printf("1. Switch to KERNEL mode\n2. Switch to USER mode\nChoice: ");
            scanf("%d", &m);
            if (m == 1) switchToKernelMode(found);
            else if (m == 2) switchToUserMode(found);
            else printf("Invalid choice.\n");
            break;
        case 21:
            terminateProcess(&r);
            break;
        case 22:
            printf("Exiting...\n");
            cleanupSystem();
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
        sleep(5);
        system("clear"); // Clear the screen after each operation
    }

    return 0;
}

