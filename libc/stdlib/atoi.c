#include <stdlib.h>

int atoi(const char *nptr) {
    int num = 0;
    int sign = 1;

    // Skip whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' ||
           *nptr == '\v' || *nptr == '\f' || *nptr == '\r') {
        nptr++;
    }

    // Check for optional sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Convert digits to integer
    while (*nptr >= '0' && *nptr <= '9') {
        num = num * 10 + (*nptr - '0');
        nptr++;
    }

    return sign * num;
}
