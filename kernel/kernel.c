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
int kmain(void) {
    // TTY - Terminal
    terminal_initialize();

    // ISR - Interrupt Service Routines
    init_isr_handlers();

    // Flush TSS
    flush_tss();

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations to avoid race conditions!
    pic_init(PIC1_OFFSET, PIC2_OFFSET);


    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    kernel_allocator.initialized = 0;
    init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);


    //printf("Mem size: %d\n", get_memory_size_from_smbios());
    

    // Kernel Heap - Manage Kernel Dynamic Memory
    printf("HEAP:\n");
    init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);

    // printf("heap: %d, dst: %d, dst2: %d\n", heap, dst, dst2);
    // char dst[30];

    // PIC - Programmable Interrupt Controller (IMPORTANT: Should be after PageFrameAllocator Init)
    // PIC should be initialized at the end of Kernel's initializations to avoid race conditions!s

    // pic_init(PIC1_OFFSET, PIC2_OFFSET);
    printf("AFTER INIT HEAP\n");

    enumerate_pci();
    printf("111\n");
    print_pci_devices();
    printf("111\n");

    enumerate_disks();
    printf("111\n");
    print_disks();
    printf("111\n");

    char buffer[] = "Hi gal";

    write(0, 0, sizeof(buffer), buffer);

    printf(buffer);
    
    __asm__ volatile ("hlt");

    return 0;
}

void test_user_function(){
    // printf("%d\n", 1);
    // __asm__ volatile("cli");
    while (true) {
    }
}