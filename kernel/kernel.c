// Kernel Main File

#include <stdio.h>
#include <stdlib.h>
#include <arch/x86_64/isr.h>
#include <string.h>

#include <arch/x86_64/tty.h>
#include <arch/x86_64/io.h>

int kmain(void) {
    install_isr_handlers();

    terminal_initialize();

    char* hello = "Hello!\nEnter message:";
    printf("%s", hello);

    char ch = 'z';
    int num = 5;
    char* dst = "hey";
    int x = scanf("%c %s %d", &ch, dst, &num);
    printf("x: %d\n", x);
    printf("%c %s %d\n", ch, dst, num);

    return 0;
}