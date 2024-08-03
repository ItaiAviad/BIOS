#include <string.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
    io_wait_fast();
	const unsigned char* src = (const unsigned char*) srcptr;
    io_wait_fast();
	for (size_t i = 0; i < size; i++) {
        io_wait_fast();
		dst[i] = src[i];
        io_wait_fast();
    }
    io_wait_fast();
	return dstptr;
}
