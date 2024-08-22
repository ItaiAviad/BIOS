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

int kmain(void) {
    // init_gdt();
    // flush_tss();

    // TTY - Terminal
    terminal_initialize();
    
    // ISR - Interrupt Service Routines
    init_isr_handlers();

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    // init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);

    // Kernel Heap - Manage Kernel Dynamic Memory
    // malloc_state* heap = (malloc_state*) init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES *
    // PAGE_SIZE); char* dst = (char*) malloc(0x1000); char* dst2 = (char*) malloc(0x2000);
    // free(dst);
    // printf("----------------\n");
    // print_heap();
    // printf("----------------\n");
    
    flush_tss();
    // char dst[30];

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations to avoid race
    // conditions! pic_init(PIC1_OFFSET, PIC2_OFFSET);

    printf("%d\n", time());
    date();

    // Jump to Userspace
    // char* hello1 = "In Kernel!\nEnter char, string and a decimal:";
    // printf("%s", hello1);
    // __asm__ volatile ("xchg bx, bx");
    // cli();
    // __asm__ volatile ("xchg bx, bx");
    // init_userspace();

    // srand(time());

    // sleep(1000);

    // char* hello = "In Kernel!\nEnter char, string and a decimal:";
    // printf("%s","hello");
    // int i = 0;
    // while (1) {
        // i++;
    // }
    // char ch = 0;
    // int num = 0;
    // int x = scanf("%c %s %d", &ch, dst, &num);
    // printf("# of parameters read: %d\n", x);
    // printf("char: %c, string: %s, decimal: %d\n", ch, dst, num);
    // printf("Division by zero interrupt: %d\n", 1 / 0);

    __asm__ volatile ("hlt");

    return 0;
}
