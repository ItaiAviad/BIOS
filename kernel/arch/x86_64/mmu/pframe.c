#include <memory.h>
#include <arch/x86_64/mmu.h>
#include <stdint.h>
#include <stdio.h>
#include <arch/x86_64/mlayout.h>
#include <string.h>

void init_page_frame_allocator(PageFrameAllocator *allocator, uint64_t memory_size) {
    // Allocate pages for the allocator via allocator_bitmap_init and then copy to the new larger allocator bitmap
    
    printf("1\n");
    if(allocator->initialized)
        return;

    printf("1+\n");
    allocator->num_pages = (memory_size / PAGE_SIZE);
    printf("1++\n");
    // allocator->bitmap = (uint8_t*) aalign(__kend, PAGE_SIZE) + PAGE_SIZE;
    allocator->bitmap = (uint8_t*) aalign(0xA000, PAGE_SIZE);
    printf("1+++\n");

    // Update __kend to the end of the new bitmap
    // __kend = aalign((uint64_t) (allocator->bitmap + (allocator->num_pages + 1) * sizeof(uint8_t)), PAGE_SIZE);
    
    printf("bitmap: %d\n", allocator->bitmap);
    printf("bitmap size: %d\n", allocator->num_pages);
    // printf("__kend: %d\n", __kend);
    printf("2\n");
    memset(allocator->bitmap, 0, allocator->num_pages); // zero bitmap
    printf("3\n");
    allocator->bitmap[0] = 1; // Set the first page as in use for dealing with NULL values

    map_important_pages((uint64_t*) PML4_KERNEL, allocator);
    printf("4\n");
    set_page_dir_reg((uint64_t *) PML4_KERNEL);
    printf("5\n");
}

void map_important_pages(uint64_t* pml4, PageFrameAllocator *allocator){
    uint64_t kend = aalign((uint64_t) 0x200000, PAGE_SIZE);
    for (uint64_t addr = 0, i = 0; addr < (kend) / PAGE_SIZE + 64; addr += PAGE_SIZE, i++) { // Identical map all of the kernels memory
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