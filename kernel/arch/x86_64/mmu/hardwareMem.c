#include <arch/x86_64/hardwareMem.h>


uint64_t hardware_alloc_size = 0;

void *hardware_allocate_mem(size_t size, size_t alignment) {
    void *addr = (void *)aalign(HARDWARE_MEM_START + hardware_alloc_size, alignment);
    hardware_alloc_size = (uint64_t)addr + size - HARDWARE_MEM_START + 1;
    map_memory_range_with_flags(&kpcb, (void*)addr, (void*)addr + size - 1, (void*)addr, PAGE_WRITE | PAGE_PRESENT | PAGE_UNCACHEABLE, 1);
    flush_tlb();
    return addr;
}
