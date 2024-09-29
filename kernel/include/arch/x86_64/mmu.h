// Manage Kernel Memory

#ifndef MMU_H

#define MMU_H

#include <types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/mlayout.h>

#define PAGE_PRESENT 0b1
#define PAGE_WRITE   0b10
#define PAGE_UNCACHEABLE 0b1000

/**
 * @brief Address Align (bigger (>) than given address)
 * 
 * @param address
 * @param alignment
 * @return uint64_t 
 */
uint64_t aalign(uint64_t, uint64_t);
uint64_t aalign_down(uint64_t addr, uint64_t alignment);
// ----------------------------------------------

typedef struct {
    uint8_t* bitmap;     // Bitmap array
    // Bitmap array: Boolean values (page used/unused). Using 8 bits for minimzed memory usage.
    //         Not using bitwise operations for simplicity over memory usage (x8 more space used for bitmap)
    uint64_t num_pages; // Total number of pages

    uint64_t initialized;
} PageFrameAllocator;

typedef struct Context {
    uint64_t start_addr; // Virtual - Relative to last context
    uint64_t kernel_start_offset; // Different for a process
    uint64_t memory_size_pages; // Total memory size (in pages) for context
    PageFrameAllocator allocator;
    uint64_t* old_pml4;
    uint64_t* pml4;
} Context;

__attribute__((unused))
Context k_ctx;
// ----------------------------------------------

// Page Frame Allocator
/**
 * @brief Init the Page Frame Allocator
 * 
 * @param allocator 
 * @param memory_size_pages Total Memory Size in Pages (for the entire OS)
 */
void init_page_frame_allocator(PageFrameAllocator* allocator, size_t memory_size);
/**
 * @brief Allocate a page to a frame
 * 
 * @param Context
 * @return void* - physical memory address
 */
void* allocate_page(Context ctx, bool is_p_struct);
void* allocate_and_zero_page(Context ctx, bool is_p_struct);
/**
 * @brief Free a page from a frame
 * 
 * @param allocator 
 * @param page physical memory address
 */
void deallocate_page(PageFrameAllocator* allocator, void* page);
// ----------------------------------------------

// Paging
/**
 * @brief Initialize Kernel Paging
 * 
 * @param allocator 
 * @param memory_size_pages Total Memory Size in Pages (for the entire OS)
 */
void init_kernel_paging(PageFrameAllocator* allocator, size_t memory_size_pages);

/**
 * @brief Switch context (Paging tables)
 * 
 * @param ctx 
 */
void switch_context(Context ctx);
/**
 * @brief Map reserved paging tables
 * 
 * @param Context
 */
void map_reserved_paging_tables(Context ctx);
/**
 * @brief Map a range of memory (in pages)
 * 
 * @param Context
 * @param start_address 
 * @param end_address 
 * @param physical_addr
 * @param allocator 
 */
void map_memory_range(Context ctx, uint64_t start_addr, uint64_t end_addr, uint64_t physical_addr);


void map_memory_range_with_flags(Context ctx, uint64_t start_addr, uint64_t end_addr, uint64_t physical_addr, uint64_t flags);

/**
 * @brief Set the PML4 Address in the CR3 reg object (Paging Register)
 * @ref https://wiki.osdev.org/CPU_Registers_x86-64#CR3
 * 
 * @param pml4 
 */
void set_pml4_address(uint64_t* pml4);

/**
 * @brief Map physical address to virtual address (Page aligned)
 * 
 * @param Context 
 * @param allocator 
 * @param virtual_address 
 * @param physical_address 
 * @param flags 
 */
void map_page(Context ctx, uint64_t virtual_address, uint64_t physical_address, uint64_t flags);

/**
 * @brief Check if a page is mapped
 * 
 * @param pml4 
 * @param virtual_address 
 * @return true 
 * @return false 
 */
bool is_page_mapped(uint64_t* pml4, uint64_t virtual_address);
// ----------------------------------------------

// Kernel Heap
/**
 * @brief Allocate a chunk of physical memory (map new page(s) if nescessary) to virtual memory
 * 
 * @param size 
 * @return void* - virtual memory address (base address for new allocation)
 */
void *kmalloc(size_t size);


#endif
