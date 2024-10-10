#include <arch/x86_64/hardwareMem.h>


uint64_t hardware_alloc_size = 0;

void *hardware_allocate_mem(size_t size, size_t alignment) {
    void *addr = (void *)aalign(HARDWARE_MEM_START + hardware_alloc_size, alignment);
    hardware_alloc_size = (uint64_t)addr + size - HARDWARE_MEM_START + 1;
    map_memory_range_with_flags(k_ctx, (uint64_t)addr, (uint64_t)addr + size - 1, (uint64_t)addr, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE, 1);
    set_pml4_address(k_ctx.pml4);
    return addr;
}
