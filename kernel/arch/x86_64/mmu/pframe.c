#include <arch/x86_64/mmu.h>

extern uint64_t __kend;

void init_page_frame_allocator(PageFrameAllocator* allocator, uint64_t memory_size) {
    allocator->num_pages = memory_size / PAGE_SIZE;
    allocator->bitmap = (uint8_t *) aalign(__kend, PAGE_SIZE); // Place bitmap after the kernel
    
    // Calculate bitmap size in bytes
    size_t bitmap_size = allocator->num_pages;
    for (size_t i = 0; i < bitmap_size; i++) {
        allocator->bitmap[i] = 0; // Initialize bitmap to all free
    }

    // Mark used pages (e.g., kernel and reserved regions)
    for (size_t i = 0; i <= (size_t)(__kend) / PAGE_SIZE; i++) {
        allocator->bitmap[i] = 1;
    }
    for (size_t i = 0; i <= bitmap_size; i++) {
        if (allocator->bitmap[i] != 1ULL) {
            break;
        }
    }
}

void* allocate_page(PageFrameAllocator* allocator) {
    for (uint64_t i = 0; i <= allocator->num_pages; i++) {
        if (allocator->bitmap[i] != 1) { // If not all bits are set
            allocator->bitmap[i] = 1; // Mark as used
            return (void *)((i) * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

void* allocate_and_zero_page(PageFrameAllocator* allocator) {
    void *page = allocate_page(allocator);
    if (page != NULL) {
        memset(page, 0, PAGE_SIZE);
    }
    return page;
}

void free_page(PageFrameAllocator* allocator, void* page) {
    uint64_t page_index = (uint64_t)page / PAGE_SIZE;
    allocator->bitmap[page_index / 64] &= ~(1ULL << (page_index % 64)); // Mark as free
}
