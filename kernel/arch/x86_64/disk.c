#include <disk.h>

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
