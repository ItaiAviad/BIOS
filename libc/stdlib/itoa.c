#include <stdlib.h>

char* itoa(int64_t num, char* str, int base) {
    int i = 0;

    if (base == 0) {
        printf("Base was 0");
        return NULL;
    }

    if (base < 2 || base > 36) {
        return NULL;
    }

    // Handle 0 explicitly, otherwise an empty string is printed for 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // If the base is 10 and the number is negative, treat it as negative
    if (num < 0 && base == 10) {
        str[i++] = '-';
        num = -num; // Make num positive
    }

    // Process individual digits
    while (num != 0) {
        if(num < 0){
            num *= -1;
        }
        uint64_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0'; // Null-terminate string

    // Reverse the string to get the correct representation
    int start = (str[0] == '-') ? 1 : 0; // Start from 1 if negative sign is present
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    return str;
}