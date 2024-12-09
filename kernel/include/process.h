#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <types.h>
#include <arch/x86_64/mlayout.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/tss.h>
#include <memory.h>

typedef struct ProcessControlBlock {
    uint32_t pid;                   // Process ID
    uint32_t state;                 // Process state (e.g., READY, RUNNING, WAITING)

    void* entry;             // Pointer to the process's code segment
    PageFrameAllocator pfa; // Page Frame Allocator
    Context ctx;            // Process Context
    
    void* stack;            // Pointer to the top of the process stack
    void* heap;               // Start address of the heap
    
    uint32_t priority;              // Priority (useful for scheduling later)
    uint64_t cpu_registers[16];     // Space for saving CPU registers during context switching
} PCB;

// Kernel ProcessControlBlock
__attribute__((unused)) struct ProcessControlBlock kpcb;

void init_kernel_process(void);


#endif
