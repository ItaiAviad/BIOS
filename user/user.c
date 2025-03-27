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
    // while (true)
    // {
    //     printf("%s Usermode\n", LOG_SYM_SUC);
    // }

    if(!fork()){
        while (1)
        {
            printf("Proc 2");
        }
        
    }
    

    // exec("/mnt/mount1/user_prog");
    shell_init();

    // printf("Now you are stuck here with me forever... :D\n");

    while (1) {}
    return 0;
}

