#include <stdlib.h>
#include <stdint.h>

// Initial state for the RNG
static uint32_t _seed = 1;

void srand(uint32_t seed) {
    _seed = seed;
}

uint32_t rand(void) {
    // Xorshift32
    uint32_t x = _seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    _seed = x;
    
    return x & 0x7FFFFFFF;
}
