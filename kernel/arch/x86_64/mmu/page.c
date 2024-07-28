#include <arch/x86_64/mmu.h>

void map_page(uint64_t* pml4, PageFrameAllocator* allocator, uint64_t virtual_address, uint64_t physical_address, uint64_t flags) {
    // Calculate indices
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_address >> 12) & 0x1FF;

    uint64_t *pdpt = NULL, *pd = NULL, *pt = NULL;

    // Ensure PDPT, PD, and PT entries exist (allocate and zero if necessary)
    if (!(pml4[pml4_index] & PAGE_PRESENT)) {
        pdpt = allocate_and_zero_page(allocator);
        pml4[pml4_index] = ((uint64_t) pdpt) | PAGE_PRESENT | PAGE_WRITE;
    } else {
        pdpt = (uint64_t*) (pml4[pml4_index] & ~0xFFF);
    }

    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        pd = allocate_and_zero_page(allocator);
        pdpt[pdpt_index] = ((uint64_t) pd) | PAGE_PRESENT | PAGE_WRITE;
    } else {
        pd = (uint64_t*) (pdpt[pdpt_index] & ~0xFFF);
    }

    if (!(pd[pd_index] & PAGE_PRESENT)) {
        pt = allocate_and_zero_page(allocator);
        pd[pd_index] = ((uint64_t) pt) | PAGE_PRESENT | PAGE_WRITE;
    } else {
        pt = (uint64_t*) (pd[pd_index] & ~0xFFF);
    }

    // Map the physical address to the virtual address in the page table
    pt[pt_index] = physical_address | flags;
}

