#include <memory.h>
#include <arch/x86_64/mmu.h>
#include <stdint.h>
#include <stdio.h>
#include <arch/x86_64/mlayout.h>
#include <string.h>

extern uint64_t __end;

void init_page_frame_allocator(PageFrameAllocator *allocator, size_t memory_size_pages) {
    cli();
    // Allocate pages for the allocator via allocator_bitmap_init and then copy to the new larger allocator bitmap
    if(allocator->initialized)
        return;

    allocator->initialized = 1;

    // Initialize the allocator
    allocator->num_pages = (memory_size_pages);
    allocator->bitmap = (uint8_t*) aalign(PAGE_FRAME_ALLOCATOR_START, PAGE_SIZE);
    memset(allocator->bitmap, (char)0, (uint64_t)allocator->num_pages); // Zero bitmap
    allocator->bitmap[0] = 1; // Set the first page as in use for dealing with NULL values
    sti();
}

void map_reserved_paging_tables(Context ctx) {
    volatile uint64_t start = aalign((uint64_t) ctx.pml4, PAGE_SIZE);
    volatile uint64_t end = aalign((uint64_t) ctx.pml4 + PAGING_SECTION_SIZE, PAGE_SIZE);
    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
        map_page(ctx, addr, addr, PAGE_MAP_FLAGS); // Identity Mapping!
        (&ctx.allocator)->bitmap[addr/PAGE_SIZE] = 0;
    }
}

void map_memory_range(Context ctx, uint64_t start_addr, uint64_t end_addr, uint64_t physical_addr) {
    volatile uint64_t start = aalign((uint64_t) start_addr, PAGE_SIZE);
    volatile uint64_t end = aalign((uint64_t) end_addr, PAGE_SIZE);
    physical_addr = aalign((uint64_t) physical_addr, PAGE_SIZE);

    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE, physical_addr += PAGE_SIZE) {
        map_page(ctx, addr, physical_addr, PAGE_MAP_FLAGS);
        (&ctx.allocator)->bitmap[addr/PAGE_SIZE] = 1;
    }
}

void *allocate_page(Context ctx, bool is_p_struct) {
    uint64_t i = 0;
    
    uint64_t pml4_start = (uint64_t)ctx.pml4 / PAGE_SIZE;
    uint64_t pml4_end = pml4_start + PAGING_SECTION_SIZE_PAGES;

    for (i = 0; i < (&ctx.allocator)->num_pages; i++) {
        // Reserve space paging structures
        if (!is_p_struct && i < pml4_end) {
            i++;
            continue;
        }
        
        if ((&ctx.allocator)->bitmap[i] != 1) { // If not all bits are set
            (&ctx.allocator)->bitmap[i] = 1;      // Mark as used
            #ifdef DEBUG
            printf("%s: Free page found: %x. Is p_struct: %d\n", DEBUG, i, is_p_struct);
            #endif
            return (void *)((i)*PAGE_SIZE);
      }
    }
    return NULL; // Out of memory
}

void *allocate_and_zero_page(Context ctx, bool is_p_struct) {
    void *page = allocate_page(ctx, is_p_struct);
    if (page != NULL) {
        memset(page, 0, PAGE_SIZE);
    }
    return page;
}

void deallocate_page(PageFrameAllocator *allocator, void *page) {
    uint64_t page_index = (uint64_t)page / PAGE_SIZE;
    allocator->bitmap[page_index / 64] &= ~(1ULL << (page_index % 64)); // Mark as free
}