#include <stdio.h>
#include <sys/syscall.h>

int umain(void) {
    // char* mymsg = "HELLO THERE";
    // init_syscall();
    syscall(sys_printf, "printf syscall: %s\n", "Hello, World!");
    // syscall(sys_getchar);
    // printf("%s\n", mymsg);
    while (1) {}
    return 0;
}

