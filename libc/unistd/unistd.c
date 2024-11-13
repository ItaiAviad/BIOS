#include <unistd.h>
#include <sys/syscall.h>

void sleep(uint64_t ms) {
#if defined(__is_libk)
    pit_sleep(ms);
#else
    syscall(sys_sleep, ms);
#endif
}
