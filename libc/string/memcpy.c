#include <string.h>
#include <sys/syscall.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
#if defined(__is_libk)
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++) {
		dst[i] = src[i];
    }

#else

    syscall(sys_memcpy, dstptr, srcptr, size);

#endif
	return dstptr;
}
