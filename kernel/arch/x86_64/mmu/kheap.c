#include <arch/x86_64/mmu.h>
#include <stdint.h>

static uint64_t heap_current = KERNEL_HEAP_START;
static uint64_t heap_end = KERNEL_HEAP_START;
static uint64_t heap_current_size_left = 0; // No allocated pages in the beginning

void *kmalloc(size_t size) {
    // Check if there is enough space for size in current allocated page
    if ((long long) heap_current_size_left - (long long) size >= 0 && heap_current_size_left > 0)
    {
        heap_current_size_left -= size;
        uint64_t allocation_addr = heap_current;
        heap_current += size;
        return (void *)allocation_addr;
    }
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; // Round up to the nearest page

    // Allocate new page(s)
    uint64_t allocation_addr = heap_current;
    for (size_t i = 0; i < num_pages; i++) {
        void *page = allocate_page(k_ctx, false);
        if (page == NULL) {
            #ifdef DEBUG
            printf("%s: GOT NULL PAGE!\n", DEBUG);
            #endif
            return NULL; // Out of memory
        }
        // Map page at end of heap
        map_page(k_ctx, heap_end, (uint64_t)page, PAGE_PRESENT | PAGE_WRITE);
        memset((void*) heap_end, 0, PAGE_SIZE);
        heap_end += PAGE_SIZE;
        heap_current_size_left += PAGE_SIZE;
    }
    heap_current_size_left -= size;
    return (void *)allocation_addr;
}


