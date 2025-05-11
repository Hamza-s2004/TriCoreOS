typedef struct ResourceManager {
    int totalRAM;
    int usedRAM;

    int totalHDD;
    int usedHDD;

    int totalCores;
    int usedCores;

    int (*allocate)(struct ResourceManager*, int ram, int hdd, int cores);
    void (*deallocate)(struct ResourceManager*, int ram, int hdd, int cores);
    void (*showStatus)(struct ResourceManager*);
} ResourceManager;

int allocate_resources(ResourceManager* r, int ram, int hdd, int cores) {
    if (r->usedRAM + ram > r->totalRAM || r->usedHDD + hdd > r->totalHDD || r->usedCores + cores > r->totalCores) {
        return 0; // Not enough resources
    }
    r->usedRAM += ram;
    r->usedHDD += hdd;
    r->usedCores += cores;
    return 1; // Allocation successful
}

void deallocate_resources(ResourceManager* r, int ram, int hdd, int cores) {
    r->usedRAM -= ram;
    r->usedHDD -= hdd;
    r->usedCores -= cores;
}

void show_resource_status(ResourceManager* r) {
    printf("RAM: %d/%d MB used\n", r->usedRAM, r->totalRAM);
    printf("HDD: %d/%d MB used\n", r->usedHDD, r->totalHDD);
    printf("Cores: %d/%d in use\n", r->usedCores, r->totalCores);
}
