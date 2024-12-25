#include "ahci.h"
#include "arch/x86_64/mmu.h"
#include <arch/x86_64/io.h>
#include <disk.h>
#include <math.h>

#include <stdlib.h>
#include <string.h>

linkedListNode *list_drives = NULL;

uint64_t last_disk_id = 0;

void enumerate_disks() {
    setup_ahci_controllers();
}

void print_disks() {
    linkedListNode *head = (linkedListNode *)list_drives;
    #ifdef DEBUG
    printf("__DRIVES__\n");
    #endif
    while (head != NULL) {
        // #ifdef DEBUG
        disk *device = (disk *)head->data;
        printf("id: %d, size: %d, type: %d\n", device->disk_id, (uint64_t)device->disk_size, device->disk_type);
        // #endif
        head = (linkedListNode *)head->next;
    }
    #ifdef DEBUG
    printf("__DRIVES__END__\n");
    #endif
}

disk *find_disk(uint64_t id) {
    linkedListNode *head = (linkedListNode *)list_drives;
    disk *device;
    while (head != NULL) {
        device = (disk *)head->data;
        if (device->disk_id == id) {
            return device;
        }
        head = (linkedListNode *)head->next;
    }

    return device;
}

void read(uint64_t disk_id, uint64_t offset, size_t size, void* buffer) {
    disk *disk = find_disk(disk_id);

    switch (disk->disk_type) {
        case AHCI_SATA: {// Use a temporary buffer in order to use non sector aligned offsets
            uint64_t offset_sectors = offset / SECTOR_SIZE;
            uint64_t size_sectors = upper_divide(size, SECTOR_SIZE);
            uint8_t* buffer_temp = malloc(size_sectors*SECTOR_SIZE);

            // Set paging section as uncacheable for dma
            map_memory_range_with_flags(kpcb.ctx, (void*)buffer_temp, (void*)buffer_temp + size_sectors*SECTOR_SIZE - 1, (void*)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE | PAGE_USER, 1);
            flush_tlb();
            read_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);
            
            map_memory_range_with_flags(kpcb.ctx, (void*)buffer_temp, (void*)buffer_temp + size_sectors*SECTOR_SIZE - 1, (void*)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_USER, 1);
            flush_tlb();

            memcpy(buffer, buffer_temp + (offset % SECTOR_SIZE), size);

            free(buffer_temp);

            break;
        }
    }
}

void write(uint64_t disk_id, uint64_t offset, size_t size, void* buffer) {

    disk *disk = find_disk(disk_id);

    switch (disk->disk_type) {
        case AHCI_SATA: {// Use a temporary buffer in order to use non sector aligned offsets
            uint64_t offset_sectors = offset / SECTOR_SIZE;
            uint64_t size_sectors = upper_divide(size, SECTOR_SIZE);
            uint8_t* buffer_temp = malloc(size_sectors*SECTOR_SIZE);

            // Its kinda a work around but should be ok.
            map_memory_range_with_flags(kpcb.ctx, (void*)buffer_temp, (void*)buffer_temp + size_sectors*SECTOR_SIZE - 1, (void*)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE | PAGE_USER, 1);
            flush_tlb();
            
            read_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);
            memcpy(buffer_temp + (offset_sectors * 512 - offset), buffer, size);
            write_ahci(disk->drive_data.ahci_drive_data.port, offset_sectors, size_sectors, buffer_temp);

            map_memory_range_with_flags(kpcb.ctx, (void*)buffer_temp, (void*)buffer_temp + size_sectors*SECTOR_SIZE - 1, (void*)buffer_temp, PAGE_WRITE | PAGE_PRESENT | PAGE_USER, 1);
            flush_tlb();

            free(buffer_temp);

            break;
        }
    }
}