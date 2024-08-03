// Memory Layout

#ifndef MLAYOUT_H

#define MLAYOUT_H

#include <types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// Paging
#define PAGE_SIZE 4096
#define MEMORY_SIZE PAGE_SIZE * 0x2000

// Paging (Kernel)
#define PML4_KERNEL 0x1000 // PML4 kernel base address

// Heap
#define KERNEL_HEAP_START 0x1000000
#define KERNEL_HEAP_SIZE_PAGES 4

// extern uint64_t __kend;
extern uint64_t __entry;

#endif
