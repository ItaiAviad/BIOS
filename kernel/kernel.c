// Kernel Main File

// libc
#include <memory.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// arch/x86_64
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/isr.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/pic.h>
#include <arch/x86_64/pit.h>
#include <arch/x86_64/tss.h>
#include <arch/x86_64/tty.h>
// sys
#include <sys/syscall.h>
#include <pci.h>
#include <disk.h>

extern void jump_usermode();
void user_init();

#ifndef USER_LOAD_ADDR
#define USER_LOAD_ADDR 0x4000000;
#endif

int kmain(void) {
    // TTY - Terminal
    terminal_initialize();
    printf("Terminal\n");

    // ISR - Interrupt Service Routines
    init_isr_handlers();
    printf("ISRs\n");

    // Flush TSS
    flush_tss();

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations to avoid race conditions!
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    printf("PIC\n");

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    kernel_allocator.initialized = 0;
    init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);
    printf("Kernel Paging\n"); 

    // Kernel Heap - Manage Kernel Dynamic Memory
    init_heap(k_ctx, KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);
    printf("Heap: %p\n", kheap_current);

    enumerate_pci();
    print_pci_devices();

    // Setup AHCI and enumerate Disks
    enumerate_disks();
    print_disks();

    // char buffer[] = "Hi gal";
    // write(0, 0, sizeof(buffer), buffer);

    // getchar();
    // usermode
    user_init();
    
    while (1) {}
    return 0;
}

void user_init() {
    size_t len = 0x20000;
    map_memory_range(k_ctx, (void*) USER_LOAD_ADDR, (void*) USER_LOAD_ADDR + len, (void*) USER_LOAD_ADDR);
    read(0, 0, len, (void*) USER_LOAD_ADDR);

    init_syscall();

    jump_usermode();
}
