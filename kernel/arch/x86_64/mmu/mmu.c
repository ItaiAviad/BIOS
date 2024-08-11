#include <arch/x86_64/mmu.h>
#include <stdint.h>

uint64_t aalign(uint64_t addr, uint64_t alignment) {
    // Ensure alignment is a power of two
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        // Handle error or invalid alignment
        return addr; // or some error value
    }
    
    return (addr + alignment - 1) & ~(alignment - 1);
}
