// Kernel Main File

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/tty.h>

int kmain(void) {
    terminal_initialize();

    char* hello = "1234";
    printf("HELLO! %%c %x %s", atoi(hello), "HEY\t");
    // terminal_scroll_line_down();

    return 0;
}