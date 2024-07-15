#include <stdio.h>

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
    ic = (int) wait_key();
#else
    // TODO: Implement stdio and the write system call.
#endif
    return ic;
}

