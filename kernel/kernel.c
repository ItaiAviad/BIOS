// Kernel Main File

// libc
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <random.h>
#include <time.h>
#include <unistd.h>
// arch/x86_64
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/isr.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/pic.h>
#include <arch/x86_64/pit.h>
#include <arch/x86_64/systemInfo.h>
#include <pci.h>
#include <ahci.h>

int kmain(void) {
    // TTY - Terminal
    terminal_initialize();
    
    // ISR - Interrupt Service Routines
    init_isr_handlers();

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);

    printf("Mem size: %d\n", get_memory_size_from_smbios());
    

    // Kernel Heap - Manage Kernel Dynamic Memory
    printf("HEAP:\n");
    malloc_state* heap = (malloc_state*) init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);

    // printf("heap: %x, dst: %x, dst2: %x\n", heap, dst, dst2);
    // char dst[30];

    // PIC - Programmable Interrupt Controller (IMPORTANT: Should be after PageFrameAllocator Init)
    // PIC should be initialized at the end of Kernel's initializations to avoid race conditions!
    pic_init(PIC1_OFFSET, PIC2_OFFSET);

    enumeratePCI();
    print_PCI_devices();

    setup_ahci_controllers();
    
    __asm__ volatile ("hlt");

    return 0;
}
