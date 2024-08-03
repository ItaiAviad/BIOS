// Memory Layout

#ifndef MLAYOUT_H

#define MLAYOUT_H

#include <types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// Paging
#define PAGE_SIZE 4096
#define MEMORY_SIZE_PAGES 0x40000
#define MEMORY_SIZE PAGE_SIZE * MEMORY_SIZE_PAGES // 1GB

// Paging (Kernel)
#define PML4_KERNEL 0x1000 // PML4 kernel base address

// Kernel Memory
#define KERNEL_SIZE 0x200 // 512 Pages
#define KERNEL_END 0x200000 // 2MB

// Kernel Page Frame Allocator
#define PAGE_FRAME_ALLOCATOR_START KERNEL_END
#define PAGE_FRAME_ALLOCATOR_BITMAP_SIZE MEMORY_SIZE_PAGES

// Heap
#define KERNEL_HEAP_START 0x1000000 // 16MB
#define KERNEL_HEAP_SIZE_PAGES 0x4 // 4 Pages

// extern uint64_t __kend;
// extern uint64_t __entry;

#endif
