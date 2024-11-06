#ifndef DISK_H
#define DISK_H

#include <pci.h>
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
    AHCI_SATA = 0
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

void enumerate_disks();
void print_disks();
disk* find_disk(uint64_t id);

/**
* @brief Writes to a disk with a certain id.
* @param[in] disk_id The id of a disk
* @param[in] offset The offset of the disk in bytes
* @param[in] size The amount of bytes to read
* @param[in] buffer The buffer to write from
*/
void write(uint64_t disk_id, uint64_t offset, size_t size, void* buffer);

/**
* @brief Reads from a disk with a certain id.
* @param[in] disk_id The id of a disk
* @param[in] offset The offset of the disk in bytes
* @param[in] size The amount of bytes to read
* @param[out] buffer The buffer to output to
*/
void read(uint64_t disk_id, uint64_t offset, size_t size, void* buffer);

#endif