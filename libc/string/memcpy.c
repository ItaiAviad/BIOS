#include <string.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
    io_wait();
	const unsigned char* src = (const unsigned char*) srcptr;
    io_wait();
	for (size_t i = 0; i < size; i++) {
        io_wait();
		dst[i] = src[i];
        io_wait();
    }
    io_wait();
	return dstptr;
}
