#include <unistd.h>

void sleep(uint64_t ms) {
#if defined(__is_libk)
    pit_sleep(ms);
#else
    // Ensure to implement the rest of the code as needed (syscall)
#endif
}
