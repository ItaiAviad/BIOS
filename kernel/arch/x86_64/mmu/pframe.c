#include <arch/x86_64/mlayout.h>
#include <arch/x86_64/mmu.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

extern uint64_t __end;

void init_page_frame_allocator(PageFrameAllocator *allocator, size_t memory_size_pages) {
    cli();
    // Allocate pages for the allocator via allocator_bitmap_init and then copy to the new larger allocator bitmap
    if (allocator->initialized)
        return;

    allocator->initialized = 1;

    // Initialize the allocator
    allocator->num_pages = (memory_size_pages);
    allocator->bitmap = (uint8_t *)(PAGE_FRAME_ALLOCATOR_START);
    memset(allocator->bitmap, (char)0, (uint64_t)allocator->num_pages); // Zero bitmap = UNNECESSARY

    memset(allocator->bitmap, (char)1, (PAGE_FRAME_ALLOCATOR_END + PAGE_SIZE) / PAGE_SIZE);

    sti();
}

void map_memory_range_with_flags(Context ctx, void* start_addr, void* end_addr, void* physical_addr, uint64_t flags, int set_in_allocator) {
    volatile uint64_t start = aalign_down((uint64_t)start_addr, PAGE_SIZE);
    volatile uint64_t end = aalign((uint64_t)end_addr, PAGE_SIZE);
    physical_addr = (void*) aalign_down((uint64_t)physical_addr, PAGE_SIZE);

    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE, physical_addr += PAGE_SIZE) {
        map_page(ctx, (void *)addr, physical_addr, flags);
        if (set_in_allocator) {
            ctx.allocator->bitmap[addr / PAGE_SIZE] = 1;
        }
    }
}

void map_memory_range(Context ctx, void* start_addr, void* end_addr, void* physical_addr) {
    map_memory_range_with_flags(ctx, start_addr, end_addr, physical_addr, PAGE_MAP_FLAGS, 1);
}

void* allocate_page(Context ctx) {
    for (uint64_t i = 0; i < (uint64_t)(ctx.allocator->num_pages); i++) {
        if (!(ctx.allocator->bitmap[i])) { // If not all bits are set
            ctx.allocator->bitmap[i] = 1;  // Mark as used
#ifdef DEBUG
            printf("%s: Free page found: %x. Is p_struct: %d\n", DEBUG, i, is_p_struct);
#endif
            return (void *)((i)*PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

void *allocate_and_zero_page(Context ctx) {
    void *page = allocate_page(ctx);
    if (page != NULL) {
        memset(page, 0, PAGE_SIZE);
    }
    return page;
}

void deallocate_page(PageFrameAllocator *allocator, void *page) {
    uint64_t page_index = (uint64_t)page / PAGE_SIZE;
    allocator->bitmap[page_index / 64] &= ~(1ULL << (page_index % 64)); // Mark as free
}


