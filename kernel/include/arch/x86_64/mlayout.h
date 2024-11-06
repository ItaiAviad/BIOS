// Memory Layout

#ifndef MLAYOUT_H

#define MLAYOUT_H



#include <stdio.h>
#include <types.h>

#define PAGE_SIZE 4096
#define MB (1024 * 1024)           // = 0x100000
#define MB_PAGES 0x100           // = 0x100000
#define GB_PAGES 0x40000         // = 0x40000 (Pages) = 1024 * 1024 * 1024 / (4096)
#define MEMORY_SIZE_PAGES (0x60 * MB) // = 2MB (Pages) = 0x200000 (Pages)
// #define MEMORY_SIZE PAGE_SIZE * MEMORY_SIZE_PAGES // 8GB = 0x200000000

// Virtual Memory:

// Kernel (Virtual) Memory
// #define KERNEL_LOAD_ADDRESS ... ==> Defined in Makefile!
#define KERNEL_END (3 * MB) // 2MB

// Kernel Page Frame Allocator
#define PAGE_FRAME_ALLOCATOR_START KERNEL_END
#define PAGE_FRAME_ALLOCATOR_BITMAP_SIZE ((MEMORY_SIZE_PAGES + PAGE_SIZE - 1) / PAGE_SIZE)
#define PAGE_FRAME_ALLOCATOR_END (PAGE_FRAME_ALLOCATOR_START + PAGE_FRAME_ALLOCATOR_BITMAP_SIZE - 1)

#define MBR_LOAD_ADDR 0x7c00

// Paging (Boot)
#define PML4_BOOT 0x80000  // PML4 kernel base address
// Paging (Kernel)
#define PML4_KERNEL 0x82000                  // PML4 kernel base address

// Heap
// #define KERNEL_HEAP_START 0x200000
#define KERNEL_HEAP_START (0x20 * MB) // 16 MB
#define KERNEL_HEAP_SIZE_PAGES 0x4  // 4 Pages

#define HARDWARE_MEM_START (KERNEL_HEAP_START + KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE)

// Processes (1GB of virtual memory: 0-0.5B -> User, 0.5B-1GB -> Kernel)
#define HIGHER_HALF_KERNEL_START_PAGES 0.5 * GB_PAGES // 0.5 GB = 0x200 (Pages)

#endif
