#ifndef DISK_H
#define DISK_H

#include "pci.h"
#include <ahci.h>
#include <arch/x86_64/hardwareMem.h>
#include <types.h>

#define SECTOR_SIZE 512

extern linkedListNode *list_drives;
extern uint64_t last_disk_id;

typedef struct ahciDriveData {
    struct HBA_PORT *port;
    struct HBA_MEM *ahci_bar;
} ahciDriveData;

typedef enum diskType {
    AHCI_SATA
} diskType;

typedef union driveData {
    ahciDriveData ahci_drive_data;
} driveData;

typedef struct disk {
    uint64_t disk_id;
    driveData drive_data;
    diskType disk_type;
    char serial_number[10];
    char model_number[20];
    uint64_t disk_size;
} disk;

void enumerateDisks();
void print_disks();

#endif