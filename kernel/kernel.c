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

int kmain(void) {
    // TTY - Terminal
    terminal_initialize();
    
    // ISR - Interrupt Service Routines
    init_isr_handlers();
    
    // PIC - Programmable Interrupt Controller
    pic_init(PIC1_OFFSET, PIC2_OFFSET);

    // Page Frame Allocator - Manage Physical Memory
    init_page_frame_allocator(&allocator, MEMORY_SIZE);

    // Kernel Heap - Manage Kernel Dynamic Memory
    malloc_state* heap = (malloc_state*) init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);
    char* dst = (char*) malloc(0x10);
    char* dst2 = (char*) malloc(0x10);
    printf("heap: %x, dst: %x, dst2: %x\n", heap, dst, dst2);
    // char dst[30];

    #ifdef UTC_YEAR
    printf("UTC year: %d\n", UTC_YEAR);
    #endif
    for (int i = 0; i < 100; i++) {
        printf("%d\n", time());
        date();
        sleep(1000);
    }

    sleep(1000);
    // printf("\n");

    char* hello = "In Kernel!\nEnter char, string and a decimal:";
    printf("%s", hello);
    char ch = 0;
    int num = 0;
    int x = scanf("%c %s %d", &ch, dst, &num);
    printf("# of parameters read: %d\n", x);
    printf("char: %c, string: %s, decimal: %d\n", ch, dst, num);
    printf("rand: %d\n", rand());
    // printf("Division by zero interrupt: %d\n", 1 / 0);

    __asm__ volatile ("hlt");

    return 0;
}
