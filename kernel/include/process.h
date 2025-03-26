#pragma once
#ifndef PROCESS_H
#define PROCESS_H




#include <linkedList.h>
#include <types.h>
#include <arch/x86_64/mlayout.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/tss.h>
#include <arch/x86_64/isr.h>



extern PCB* current_pcb;

#define MAX_NUM_OF_PROCESS 256

#define KERNEL_PID 1

#define BITMASK(n) (1 << (n))

typedef struct linkedListNode linkedListNode;


extern uint64_t process_pid_bitmap;

extern struct linkedListNode* pcb_list;

typedef struct __attribute__((packed)) {
    // Define the callee-saved registers
    uint64_t r15, r14, r13, r12, rbp, rbx;

    // Define the callee-clobbered registers
    uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;

    uint64_t eflags, rip, rsp;
} cpu_state;

typedef enum proc_state {
    BUILDING, READY, WAITING, ZOMBIE
} proc_state;

typedef struct ProcessControlBlock {
    uint32_t ppid;                   // Parent process ID
    uint32_t pid;                   // Process ID
    proc_state state;                 // Process state (e.g., READY, RUNNING, WAITING)

    void* real_mem_addr;
    void* entry;             // Pointer to the process's code segment
    PageFrameAllocator pfa; // Page Frame Allocator
    Context ctx;            // Process Context
    
    void* kernel_stack;            // The current kernel stack for the process
    void* stack;            // Pointer to the top of the process stack
    void* heap;               // Start address of the heap
    
    uint32_t priority;              // Priority (useful for scheduling later)
    cpu_state cpu_context;     // CPU state for context switching
    linkedListNode* list_node;
} PCB;

// Kernel ProcessControlBlock
__attribute__((unused)) struct ProcessControlBlock kpcb;


void init_kernel_process(void);


uint64_t allocate_pid();
void dealloc_pid(uint64_t pid);


uint64_t allocate_proc_mem();
void deallocate_proc_mem(uint64_t addr);

PCB* alloc_proc();

int load_proc_mem(PCB* pcb, char* path_to_elf);

PCB* find_pcb_by_pid(uint64_t pid);

int run_proc(PCB* pcb);

uint64_t get_current_proc_kenrel_addr();

void set_current_proc_kenrel_addr(uint64_t addr);

#endif
