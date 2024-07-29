#include <memory.h>
#include <arch/x86_64/mmu.h>
#include <stdint.h>
#include <stdio.h>
#include <arch/x86_64/mlayout.h>

void init_page_frame_allocator(PageFrameAllocator *allocator, uint64_t memory_size) {
  allocator->num_pages = memory_size / PAGE_SIZE;
  // Calculate bitmap size in bytes
  size_t bitmap_size = allocator->num_pages;
  allocator->bitmap = (uint8_t*)aalign(__kend, PAGE_SIZE);
  memset(allocator->bitmap, 0, bitmap_size*sizeof(uint8_t));// zero bitmap
  __kend = (uint64_t)(allocator->bitmap + bitmap_size*sizeof(uint8_t));
  for (uint64_t addr = 0, i = 0; addr < __kend; addr += PAGE_SIZE, i++) { // Allocate pages with identical mappings map allkernel memory
      map_page((uint64_t *)PML4_KERNEL, allocator, addr, addr, PAGE_PRESENT | PAGE_WRITE);
      allocator->bitmap[i] = 1;
  }
  set_page_dir_reg((uint64_t *)PML4_KERNEL);
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
