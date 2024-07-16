// Kernel Main File

#include <stdio.h>
#include <stdlib.h>
#include <arch/x86_64/isr.h>
#include <string.h>

#include <arch/x86_64/tty.h>

int kmain(void) {
    install_isr_handlers();

    terminal_initialize();
    char* hello = "1234";
    printf("HELLO! %%c %x %s", atoi(hello), "HEY\t");
    int test_int = 1/0;
    // terminal_scroll_line_down();

    return 0;
}