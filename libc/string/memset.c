#include <sys/syscall.h>
#include <string.h>

void *memset(void *bufptr, char value, size_t size) {
#if defined(__is_libk)

    unsigned char *buf = (unsigned char *)bufptr;
    for (size_t i = 0; i < size; i++) {
        buf[i] = (unsigned char)value;
    }

#else

    syscall(sys_memset, bufptr, value, size);

#endif
    return bufptr;
}
