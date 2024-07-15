// Kernel Main File

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/tty.h>

int kmain(void) {
    terminal_initialize();

    char* hello = "1234";
    printf("HELLO! %%c %s %d %x", "HEY", 0, atoi(hello));

    return 0;
}