// Memory Layout

#ifndef MLAYOUT_H

#define MLAYOUT_H



#include <stdio.h>
#include <types.h>

#define PAGE_SIZE 4096
#define MB (1024 * 1024)           // = 0x100000
#define MB_PAGES 0x100           // = 0x100000
#define GB_PAGES 0x40000         // = 0x40000 (Pages) = 1024 * 1024 * 1024 / (4096)
#define MEMORY_SIZE_PAGES (0x40 * MB) // = 1GB / 4 memory
#define MEMORY_SIZE MEMORY_SIZE_PAGES / PAGE_SIZE
// #define MEMORY_SIZE PAGE_SIZE * MEMORY_SIZE_PAGES // 8GB = 0x200000000


// Virtual Memory:

// Kernel (Virtual) Memory

// #define KERNEL_LOAD_ADDRESS ... ==> Defined in Makefile!
#ifndef KERNEL_LOAD_ADDR
#define KERNEL_LOAD_ADDR 0x10000
#endif
#ifndef KERNEL_VBASE
#define KERNEL_VBASE 0x410000 // Kernel binary VA
#endif
#define KERNEL_STACK 0xF000

#define KERNEL_END (6 * MB) // 2MB

// Kernel Page Frame Allocator
#define PAGE_FRAME_ALLOCATOR_START KERNEL_END
#define PAGE_FRAME_ALLOCATOR_BITMAP_SIZE (MEMORY_SIZE_PAGES)
#define PAGE_FRAME_ALLOCATOR_END (PAGE_FRAME_ALLOCATOR_START + PAGE_FRAME_ALLOCATOR_BITMAP_SIZE - 1)

#define BINARY_CODE_OFFSET 0x0
#define BINARY_CODE_SIZE (2 * MB)

#define MBR_LOAD_ADDR 0x7c00

// Paging (Boot)
#define PML4_BOOT 0x80000  // PML4 kernel base address
// Paging (Kernel)
#define PML4_KERNEL 0x82000                  // PML4 kernel base address

#define KERNEL_STACK_START PML4_BOOT

// Heap
// #define KERNEL_HEAP_START 0x200000
#define KERNEL_HEAP_START (0x20 * MB) // 16 MB
#define KERNEL_HEAP_SIZE_PAGES 0x400  // 1024 Pages

#define HARDWARE_MEM_START (KERNEL_HEAP_START + KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE)

#ifndef USER_LOAD_ADDR
#define USER_LOAD_ADDR 0x4000000
#endif

// Processes (1GB of virtual memory: 0-0.5B -> User, 0.5B-1GB -> Kernel)
#define HIGHER_HALF_KERNEL_START_PAGES 0.5 * GB_PAGES // 0.5 GB = 0x200 (Pages)


// Process memory layout (see ProcessMemoryLayout.md)
#define PROC_MEM_SIZE PAGE_SIZE * 0x800 // 8MB
#define PROC_BIN_ADDR 0x0
#define PROC_BIN_SIZE PAGE_SIZE * 0x20 // 128KB

#define PROC_SLOT_SIZE PAGE_SIZE * 0x20 // 128KB

#define PROC_PFA_ADDR PROC_BIN_ADDR + PROC_BIN_SIZE
#define PROC_PFA_SIZE PROC_SLOT_SIZE

#define PROC_PML4T_ADDR PROC_PFA_ADDR + PROC_PFA_SIZE

#define PROC_STACK_SIZE PROC_SLOT_SIZE
#define PROC_HEAP_SIZE PROC_SLOT_SIZE

#define PROC_KERNEL_ADDR PROC_MEM_SIZE / 2
#define PROC_KERNEL_SIZE PROC_MEM_SIZE / 2

#define PROC_SLOTS_OFFSET 8 // save slots for binary, pfa, pml4t
#define PROC_SLOTS PROC_MEM_SIZE / 2 / (PROC_STACK_SIZE) - PROC_SLOTS_OFFSET

#endif
