// Kernel Main File

// libc
#include <memory.h>
#include <random.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
// arch/x86_64
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/isr.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/pic.h>
#include <arch/x86_64/pit.h>
#include <arch/x86_64/tss.h>
#include <arch/x86_64/tty.h>

extern void jump_usermode();

void print_gdtr() {
    // GDTR register structure
    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) gdtr;

    // Use inline assembly to load GDTR
    __asm__ (
        "sgdt %0"   // Store GDTR register into the memory pointed to by %0
        : "=m" (gdtr)  // Output operand: GDTR structure
    );

    // Print GDTR base and limit
    printf("GDTR Base Address: %d\n", gdtr.base);
    printf("GDTR Limit: %d\n", gdtr.limit);
}

int kmain(void) {
    // ISR - Interrupt Service Routines

    // TTY - Terminal
    //init_gdt();
    terminal_initialize();
    init_isr_handlers();
    flush_tss();
    pic_init(PIC1_OFFSET, PIC2_OFFSET);


    kernel_allocator.initialized = 0;
    //init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);

    jump_usermode();

    __asm__ volatile ("hlt");

    

    return 0;
}


void test_user_function(){
    while (true) {
    }
}