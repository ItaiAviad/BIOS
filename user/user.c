#include <stdio.h>
#include <sys/syscall.h>

int umain(void) {
    // char* mymsg = "HELLO THERE";
    // init_syscall();
    syscall(sys_printf, 0x41, 0x42, 0x43);
    // printf("%s\n", mymsg);
    while (1) {}
    return 0;
}

