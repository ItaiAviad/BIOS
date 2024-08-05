#include <stdint.h>
#include <string.h>

void* memset(uint8_t* bufptr, char value, uint8_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++) {

		*(buf+i) = (unsigned char) value;
    }
	return bufptr;
}
