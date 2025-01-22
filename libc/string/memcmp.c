#include <string.h>

int memcmp(const unsigned char* a, const unsigned char *b, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (a[i] < b[i])
            return -1;
        else if (b[i] < a[i])
            return 1;
    }
    return 0;
}
