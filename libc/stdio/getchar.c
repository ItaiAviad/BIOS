#include <stdio.h>
#include <sys/syscall.h>

#if defined(__is_libk)
#include <arch/x86_64/tty.h>
#include <arch/x86_64/io.h>
#endif

/**
 * @brief Reads a character from stdin
 * 
 * @return int - char
 */
int getchar() {
    int ic = 0;
#if defined(__is_libk)
    sti();
    ic = (int) wait_key();
    cli();
#else
    ic = syscall(sys_getchar);
#endif
    return ic;
}

