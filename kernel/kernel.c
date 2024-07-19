// Kernel Main File

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <random.h>
#include <math.h>
#include <memory.h>

#include <arch/x86_64/isr.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/io.h>

int kmain(void) {
    // ISR - Interrupt Service Routines
    install_isr_handlers();

    // TTY - Terminal
    terminal_initialize();
    
    malloc_state* heap = (malloc_state*) init_heap(0x100000, 0x100000);
    char* dst = (char*) malloc(0x10);
    printf("malloc: %d\n", dst);
    // char dst[33];

    printf("malloc chunk size: %d\n", sizeof(malloc_chunk));
    char* hello = "In Kernel!\nEnter char, string and a decimal:";
    printf("%s", hello);
    char ch = 0;
    int num = 0;
    int x = scanf("%c %s %d", &ch, dst, &num);
    printf("# of parameters read: %d\n", x);
    printf("char: %c, string: %s, decimal: %d\n", ch, dst, num);
    printf("rand: %d\n", rand());
    // printf("Division by zero interrupt: %d\n", 1 / 0);

    return 0;
}