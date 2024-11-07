#include <stdio.h>
#include <sys/syscall.h>

int umain(void) {
    // char* mymsg = "HELLO THERE";
    // init_syscall();
    syscall(sys_printf, 0, 0, 0);
    // printf("%s\n", mymsg);
    while (1) {}
    return 0;
}

