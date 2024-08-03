// Manage Kernel Memory

#ifndef MMU_H

#define MMU_H

#include <types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "mlayout.h"

#define PAGE_PRESENT 0b1
#define PAGE_WRITE   0b10

typedef struct {
    uint8_t* bitmap;     // Bitmap array
    // Bitmap array: Boolean values (page used/unused). Using 8 bits for minimzed memory usage.
    //         Not using bitwise operations for simplicity over memory usage (x8 more space used for bitmap)
    uint64_t num_pages; // Total number of pages

    uint64_t initialized;
} PageFrameAllocator;

/**
 * @brief Address Align (bigger (>) than given address)
 * 
 * @param address
 * @param alignment
 * @return uint64_t 
 */
uint64_t aalign(uint64_t, uint64_t);

/**
 * @brief Init the Page Frame Allocator
 * 
 * @param allocator 
 * @param memory_size 
 */
void init_page_frame_allocator(PageFrameAllocator* allocator, uint64_t memory_size);
/**
 * @brief Allocate a page to a frame
 * 
 * @param allocator 
 * @return void* - physical memory address
 */
void* allocate_page(PageFrameAllocator* allocator);
void* allocate_and_zero_page(PageFrameAllocator* allocator);
void map_important_pages(uint64_t* pml4 ,PageFrameAllocator *allocator);
/**
 * @brief Free a page from a frame
 * 
 * @param allocator 
 * @param page physical memory address
 */

void set_page_dir_reg(uint64_t* pml4);

void free_page(PageFrameAllocator* allocator, void* page);

void map_page(uint64_t* pml4, PageFrameAllocator* allocator, uint64_t virtual_address, uint64_t physical_address, uint64_t flags);

bool check_if_mapped(uint64_t* pml4, uint64_t virtual_address);

// Kernel Heap
/**
 * @brief Allocate a chunk of physical memory (map new page(s) if nescessary) to virtual memory
 * 
 * @param allocator Page Frame Allocator
 * @param size 
 * @return void* - virtual memory address (base address for new allocation)
 */
void *kmalloc(uint64_t* pml4, PageFrameAllocator* allocator, size_t size);


#endif
