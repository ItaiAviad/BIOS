#include <stdlib.h>

char* itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    if (base < 2 || base > 36) {
        return NULL;
    }
    io_wait();

    // Handle 0 explicitly, otherwise empty string is printed for 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    io_wait();

    // In standard itoa(), negative numbers are handled only with base 10
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
    io_wait();

    // Process individual digits
    while (num != 0) {
        io_wait_fast();
        int rem = num % base;
        io_wait_fast();
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        io_wait_fast();
        num = num / base;
    }
    io_wait();

    // Append negative sign for base 10 numbers
    if (isNegative) {
        str[i++] = '-';
    }
    io_wait();

    str[i] = '\0'; // Null-terminate string

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        io_wait_fast();
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    return str;
}
