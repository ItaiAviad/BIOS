#include <arch/x86_64/mmu.h>

uint64_t aalign(uint64_t addr, uint64_t alignment) {
    // Align addr by alignment
    if (addr % alignment == 0)
        return addr;
    return (((int) (addr / alignment + 1)) * alignment);
}
