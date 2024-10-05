#include <random.h>


// Initial state for the RNG
static uint32_t _seed = 1;

void srand(uint32_t seed) {
    _seed = seed;
}

// uint32_t rand(void) {
//     // Xorshift32
//     uint32_t x = _seed;
//     x ^= x << 13;
//     x ^= x >> 17;
//     x ^= x << 5;
//     _seed = x;
    
//     return x & 0x7FFFFFFF;
// }

uint32_t rand(void) {
    // GLIBC pseudo-random implementation
    // See: https://www.mscs.dal.ca/~selinger/random/
    
    const int MAX = 1000;
    int r[MAX];
    int i;
    r[0] = _seed;
    for (i = 1; i < 31; i++) {
        r[i] = (16807LL * r[i - 1]) % 2147483647;
        if (r[i] < 0)
            r[i] += 2147483647;
    }
    for (i = 31; i < 34; i++) {
        r[i] = r[i - 31];
    }
    for (i = 34; i < 344; i++) {
        r[i] = r[i - 31] + r[i - 3];
    }
    for (i = 344; i < MAX; i++) {
        r[i] = r[i - 31] + r[i - 3];
        _seed = r[i];
        return (((unsigned int) r[i]) >> 1);
    }
    return (((unsigned int) r[344]) >> 1);
}
