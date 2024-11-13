#include <stdio.h>
#include <sys/syscall.h>

int umain(void) {
    printf("NUM: %d\n", 0x41);
    printf("NUM: %d\n", 0x41);
    int x = 1 / 0;
    printf("Enter name: ");
    // char name[100];
    // scanf("%s", name);

    while (1) {}
    return 0;
}

