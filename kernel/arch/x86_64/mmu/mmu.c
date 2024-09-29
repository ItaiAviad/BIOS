#include <arch/x86_64/mmu.h>

uint64_t aalign(uint64_t addr, uint64_t alignment) {
    // Align addr by alignment
    if (alignment == 0 || addr % alignment == 0)
        return addr;
    return ((addr / alignment + 1) * alignment);
}

uint64_t aalign_down(uint64_t addr, uint64_t alignment) {
    // Align addr by alignment
    if (alignment == 0 || addr % alignment == 0)
        return addr;
    return ((addr / alignment) * alignment);
}

