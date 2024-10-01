#include <math.h>

int pow(int base, int exp) {
    int result = 1;
    while (exp > 0) {
        result *= base;
        exp--;
    }
    return result;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a < b ? b : a;
}

int upper_divide(int numerator, int denominator) {
    return (numerator + denominator - 1) / denominator;
}

