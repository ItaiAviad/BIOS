#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <random.h>

#include <shell.h>
#include <kernel.h>

int umain(void) {
    printf("%s Usermode\n", LOG_SYM_SUC);
    // get rsp with inline assembly and printf its value

    shell_init();

    while (1) {}
    return 0;
}

