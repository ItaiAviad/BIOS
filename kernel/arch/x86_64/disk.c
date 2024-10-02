#include "ahci.h"
#include "arch/x86_64/mmu.h"
#include <arch/x86_64/io.h>
#include <disk.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

linkedListNode *list_drives = NULL;

uint64_t last_disk_id = 0;

void enumerateDisks() {
    setup_ahci_controllers();
}

void print_disks() {
    linkedListNode *head = (linkedListNode *)list_drives;
    printf("__DRIVES__\n");
    while (head != NULL) {
        disk *device = (disk *)head->data;
        printf("Id: %d, size: %d\n", device->disk_id, (uint64_t)device->disk_size);
        head = (linkedListNode *)head->next;
    }
    printf("__DISKS_END__\n");
}

disk *find_disk(uint64_t id) {
    linkedListNode *head = (linkedListNode *)list_drives;
    while (head != NULL) {
        disk *device = (disk *)head->data;
        if (device->disk_id == id) {
            return device;
        }
        head = (linkedListNode *)head->next;
    }
}

void read(uint64_t disk_id, uint64_t offset, size_t size, uint8_t* buffer) {

    disk *disk = find_disk(disk_id);

    switch (disk->disk_type) {
        case AHCI_SATA: {// Use a temporary buffer in order to use non sector aligned offsets
            uint64_t offset_sectors = offset / SECTOR_SIZE;
            uint64_t size_sectors = upper_divide(size, SECTOR_SIZE);
            uint8_t* buffer_temp = malloc(size_sectors*SECTOR_SIZE);

            // Its kinda a work around but should be ok.
            map_memory_range_with_flags(k_ctx, (uint64_t)buffer_temp, (uint64_t)buffer_temp + size_sectors*SECTOR_SIZE - 1, (uint64_t)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE);
            set_pml4_address(k_ctx.pml4);
            
            read_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);
            

            map_memory_range_with_flags(k_ctx, (uint64_t)buffer_temp, (uint64_t)buffer_temp + size_sectors*SECTOR_SIZE - 1, (uint64_t)buffer_temp, PAGE_WRITE | PAGE_PRESENT);
            set_pml4_address(k_ctx.pml4);

            memcpy(buffer, buffer_temp + (offset_sectors * SECTOR_SIZE - offset), size);

            free(buffer_temp);

            break;
        }
    }
}

void write(uint64_t disk_id, uint64_t offset, size_t size, uint8_t* buffer) {

    disk *disk = find_disk(disk_id);

    switch (disk->disk_type) {
        case AHCI_SATA: {// Use a temporary buffer in order to use non sector aligned offsets
            uint64_t offset_sectors = offset / SECTOR_SIZE;
            uint64_t size_sectors = upper_divide(size, SECTOR_SIZE);
            uint8_t* buffer_temp = malloc(size_sectors*SECTOR_SIZE);

            // Its kinda a work around but should be ok.
            map_memory_range_with_flags(k_ctx, (uint64_t)buffer_temp, (uint64_t)buffer_temp + size_sectors*SECTOR_SIZE - 1, (uint64_t)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE);
            set_pml4_address(k_ctx.pml4);

            read_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);
            memcpy(buffer_temp + (offset_sectors * 512 - offset), buffer, size);
            write_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);

            map_memory_range_with_flags(k_ctx, (uint64_t)buffer_temp, (uint64_t)buffer_temp + size_sectors*SECTOR_SIZE - 1, (uint64_t)buffer_temp, PAGE_WRITE | PAGE_PRESENT);
            set_pml4_address(k_ctx.pml4);

            free(buffer_temp);

            break;
        }
    }
}
