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
#include <arch/x86_64/pic.h>

int kmain(void) {
    // PIC - Programmable Interrupt Controller
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    irq_clear_mask(IRQ_KEYBOARD);
    irq_clear_mask(0x1);

    // ISR - Interrupt Service Routines
    init_isr_handlers();

    // TTY - Terminal
    terminal_initialize();
    
    malloc_state* heap = (malloc_state*) init_heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);
    printf("after heap init\n");
    char* dst = (char*) malloc(0x10);
    char* dst2 = (char*) malloc(0x10);
    printf("dst: %x, dst2: %x\n", dst, dst2);
    // while (1){
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
        printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
    // }
    // char dst[33];

    char* hello = "In Kernel!\nEnter char, string and a decimal:";
    printf("%s", hello);
    char ch = 0;
    int num = 0;
    int x = scanf("%c %s %d", &ch, dst, &num);
    printf("# of parameters read: %d\n", x);
    printf("char: %c, string: %s, decimal: %d\n", ch, dst, num);
    printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
    printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
    printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
    printf("pic_get_irr: %b, pic_get_isr: %b\n", pic_get_irr(), pic_get_isr());
    // printf("rand: %d\n", rand());
    // printf("Division by zero interrupt: %d\n", 1 / 0);

    return 0;
}