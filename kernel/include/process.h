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
#include <arch/x86_64/isr.h>

#define MAX_NUM_OF_PROCESS 256

#define KERNEL_PID 1

#define BITMASK(n) (1 << (n))

extern uint64_t process_pid_bitmap;

typedef struct __attribute__((packed)) {
    // Define the callee-saved registers
    uint64_t cr2, r15, r14, r13, r12, rbp, rbx;

    // Define the callee-clobbered registers
    uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;

    uint64_t rip, cs, eflags, rsp, ss;
} cpu_state;

typedef struct ProcessControlBlock {
    uint32_t ppid;                   // Parent process ID
    uint32_t pid;                   // Process ID
    uint32_t state;                 // Process state (e.g., READY, RUNNING, WAITING)

    void* real_mem_addr;
    void* entry;             // Pointer to the process's code segment
    PageFrameAllocator pfa; // Page Frame Allocator
    Context ctx;            // Process Context
    
    void* stack;            // Pointer to the top of the process stack
    void* heap;               // Start address of the heap
    
    uint32_t priority;              // Priority (useful for scheduling later)
    cpu_state cpu_context;     // CPU state for context switching
} PCB;

// Kernel ProcessControlBlock
__attribute__((unused)) struct ProcessControlBlock kpcb;

void init_kernel_process(void);


uint64_t allocate_pid();
void dealloc_pid(uint64_t pid);


uint64_t allocate_proc_mem();
void deallocate_proc_mem(uint64_t addr);

PCB* alloc_proc(uint64_t ppid);



#endif
