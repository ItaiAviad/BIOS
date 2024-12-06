// Kernel Main File

// libc
#include <memory.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// arch/x86_64
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/isr.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/pic.h>
#include <arch/x86_64/pit.h>
#include <arch/x86_64/tss.h>
#include <arch/x86_64/tty.h>
// sys
#include <sys/syscall.h>
// drivers
#include <pci.h>
#include <disk.h>
// process
#include <elf.h>
#include <process.h>

extern void jump_usermode(void* entry, void* sp);
void user_init();

int kmain(void) {
    // TTY - Terminal
    terminal_initialize();
    printf("%s Terminal\n", LOG_SYM_SUC);

    // ISR - Interrupt Service Routines
    init_isr_handlers();
    printf("%s ISRs\n", LOG_SYM_SUC);

    // Flush TSS
    flush_tss();

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations to avoid race conditions!
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    printf("%s PIC\n", LOG_SYM_SUC);

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    kernel_allocator.initialized = 0;
    init_kernel_paging(&kernel_allocator, MEMORY_SIZE_PAGES);
    printf("%s Kernel Paging\n", LOG_SYM_SUC);

    // Kernel Heap - Manage Kernel Dynamic Memory
    init_heap(k_ctx, KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);
    printf("%s Heap: %p\n", LOG_SYM_SUC, kheap_current);

    // Init PCI
    enumerate_pci();

    // Setup AHCI and enumerate Disks
    enumerate_disks();

    cli();

    // Init Syscall
    init_syscall();
    printf("%s Syscall\n", LOG_SYM_SUC);

    srand(time());

    // usermode
    user_init();
    
    while (1) {}
    return 0;
}

void user_init() {
    cli();

    struct ProcessControlBlock pcb = {
        .pid = 0,
        .state = 0,

        .entry = (void*) USER_LOAD_ADDR,
        .pfa = {
            .bitmap = pcb.entry + PROC_PFA_ADDR,
            .num_pages = PROC_MEM_SIZE / PAGE_SIZE,
            .initialized = 1
        },
        .ctx = {
            .start_addr = (uint64_t) pcb.entry,
            .kernel_addr = (uint64_t) pcb.entry + PROC_KERNEL_ADDR,
            .memory_size_pages = pcb.pfa.num_pages,
            .allocator = &pcb.pfa,
            .old_pml4 = (uint64_t*) PML4_KERNEL,
            .pml4 = pcb.entry + PROC_PML4T_ADDR
        },

        .stack = 0,
        .heap = 0,

        .priority = 0,
        .cpu_registers[0] = 0
    };

    // Map process memory in kernel's PML4T
    map_memory_range(k_ctx, (void*) pcb.entry, (void*) pcb.entry + PROC_MEM_SIZE, (void*) pcb.entry);

    // Read binary into process memory
    read(0, 0, PROC_BIN_SIZE, (void*) pcb.entry);

    // PML4T
    k_ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)pcb.ctx.pml4 | (uint64_t)PAGE_MAP_FLAGS;
    init_recursive_paging(pcb.ctx);
    flush_tlb();
    invlpg((uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM,PML4_RECURSIVE_ENTRY_NUM));
    
    // Mark process binary, pfa as allocated
    memset(pcb.ctx.allocator->bitmap, 0x1, upper_divide(PROC_PML4T_ADDR, PAGE_SIZE));

    // Map process binary, pfa
    map_memory_range(pcb.ctx, (void*) PROC_BIN_ADDR, (void*) PROC_PML4T_ADDR, (void*) pcb.entry);
    // Map process PML4T
    map_memory_range(pcb.ctx, (void*) PROC_PML4T_ADDR, (void*) (PROC_PML4T_ADDR + PAGE_SIZE), (void*)pcb.ctx.pml4);

    // Stack
    int stack_slot = rand() % PROC_SLOTS + PROC_SLOTS_OFFSET;
    while (stack_slot < PROC_SLOTS_OFFSET)
        stack_slot = rand() % PROC_SLOTS + PROC_SLOTS_OFFSET;
    pcb.stack = (void*) pcb.entry + stack_slot * PROC_SLOT_SIZE;

    // Heap
    int heap_slot = rand() % PROC_SLOTS + PROC_SLOTS_OFFSET;
    while (heap_slot < PROC_SLOTS_OFFSET || abs(stack_slot - heap_slot) <= 1)
        heap_slot = rand() % PROC_SLOTS + PROC_SLOTS_OFFSET;
    pcb.heap = (void*) pcb.entry + heap_slot * PROC_SLOT_SIZE;
    // TODO: Update heap_malloc_state_base so kernel mallocs in process's heap

    // Map Kernel (higher half)

    // set_rsp(USER_LOAD_ADDR);

    printf("%p, %p, %p\n", pcb.entry, pcb.stack, pcb.heap);

    // Switch PML4 to use the (new) s PML4
    // set_pml4_address((uint64_t *) pcb.ctx.pml4);
    printf("printf addr: %p\n", printf);

    sti();
    jump_usermode((void*)pcb.entry, (void*)pcb.stack);
}
