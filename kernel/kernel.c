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
    //cli();
    terminal_initialize();
    //init_gdt();
    init_isr_handlers();
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    //sti();
    kernel_allocator.initialized = 0;
    init_page_frame_allocator(&kernel_allocator, MEMORY_SIZE_PAGES);

    // init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);
    // 
    // sti();
    char c = '1';
    buffer_get();
    //__asm__ volatile ("xchg bx, bx");
    //flush_tss();*/




    //print_gdtr();

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    //init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);

    // Kernel Heap - Manage Kernel Dynamic Memory
    // malloc_state* heap = (malloc_state*) init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES *
    // PAGE_SIZE); char* dst = (char*) malloc(0x1000); char* dst2 = (char*) malloc(0x2000);
    // free(dst);
    // printf("----------------\n");
    // print_heap();
    // printf("----------------\n");
    
    // int dst;
    // scanf("%d", &dst);

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations to avoid race
    // conditions! pic_init(PIC1_OFFSET, PIC2_OFFSET);

    // printf("%d\n", dst);
    //date();

    // Jump to Userspace
    // char* hello1 = "In Kernel!\nEnter char, string and a decimal:";
    // printf("%s", hello1);
    // __asm__ volatile ("xchg bx, bx");
    // cli();
    // __asm__ volatile ("xchg bx, bx");
    // init_userspace();

    // srand(time());

    // sleep(1000);

    char* hello = "In Kernel!\nEnter char, string and a decimal:";
    printf("%s",hello);
    int i = 0;
    while (1) {
        i++;
        printf("%d", i);
    }
    // char ch = 0;
    // int num = 0;
    // int x = scanf("%c %s %d", &ch, dst, &num);
    // printf("# of parameters read: %d\n", x);
    // printf("char: %c, string: %s, decimal: %d\n", ch, dst, num);
    // printf("Division by zero interrupt: %d\n", 1 / 0);

    __asm__ volatile ("hlt");

    return 0;
}
