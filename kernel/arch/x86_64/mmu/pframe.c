#include <memory.h>
#include <arch/x86_64/mmu.h>
#include <stdint.h>
#include <stdio.h>
#include <arch/x86_64/mlayout.h>
#include <string.h>

int allocator_initialized = 0;
uint8_t allocator_bitmap_init[MEMORY_SIZE_INIT/PAGE_SIZE];

void init_page_frame_allocator(PageFrameAllocator *allocator, uint64_t memory_size) {
    // Allocate pages for the allocator via allocator_bitmap_init and then copy to the new larger allocator bitmap

    uint64_t init_bitmap_size = sizeof(allocator_bitmap_init) * sizeof(uint8_t);
    allocator->num_pages = init_bitmap_size;

    size_t new_bitmap_size = (memory_size/PAGE_SIZE) * sizeof(uint8_t);
    allocator->bitmap = allocator_bitmap_init;

    uint8_t* new_bitmap_start_addr = (uint8_t*) aalign(__kend, PAGE_SIZE);
    __kend = (uint64_t)(new_bitmap_start_addr + new_bitmap_size * sizeof(uint8_t));

    memset(allocator->bitmap, 0, allocator->num_pages * sizeof(uint8_t)); // zero bitmap
    allocator->bitmap[0] = 1; // Set the first page as in use for dealing with NULL values
    map_important_pages((uint64_t*) PML4_KERNEL, allocator);
    set_page_dir_reg((uint64_t *) PML4_KERNEL);

    memcpy(allocator->bitmap, allocator_bitmap_init, init_bitmap_size);
    allocator->bitmap = new_bitmap_start_addr;
    allocator->num_pages = new_bitmap_size;
}

void map_important_pages(uint64_t* pml4 ,PageFrameAllocator *allocator){
    for (uint64_t addr = 0, i = 0; addr < (__kend) / PAGE_SIZE; addr += PAGE_SIZE, i++) { // Identical map all of the kernels memory
        map_page(pml4, allocator, addr, addr, PAGE_PRESENT | PAGE_WRITE);
        allocator->bitmap[i] = 1;
    }
}

void *allocate_page(PageFrameAllocator *allocator) {
    for (uint64_t i = 0; i < allocator->num_pages; i++) {
        if (allocator->bitmap[i] != 1) { // If not all bits are set
            allocator->bitmap[i] = 1;      // Mark as used
            return (void *)((i)*PAGE_SIZE);
      }
    }
    return NULL; // Out of memory
}

void *allocate_and_zero_page(PageFrameAllocator *allocator) {
    void *page = allocate_page(allocator);
    if (page != NULL) {
        memset(page, 0, PAGE_SIZE);
    }
    return page;
}

void free_page(PageFrameAllocator *allocator, void *page) {
    uint64_t page_index = (uint64_t)page / PAGE_SIZE;
    allocator->bitmap[page_index / 64] &= ~(1ULL << (page_index % 64)); // Mark as free
}
