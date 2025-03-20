#include <process.h>
#include <arch/x86_64/interrupts.h>

uint64_t process_pid_bitmap = 0;
uint64_t process_mem_bitmap = 0;

uint64_t allocate_pid(){
    for(int i = 0; i < MAX_NUM_OF_PROCESS; i++){
        if(!(process_pid_bitmap & BITMASK(i)) && i != 0 && i != KERNEL_PID){
            process_pid_bitmap |= BITMASK(i);
            return i;
        }
    }
    return 0;
}

void deallocate_pid(uint64_t pid){
        process_pid_bitmap &= ~BITMASK(pid);
}


uint64_t allocate_proc_mem(){
    for(int i = 0; i < MAX_NUM_OF_PROCESS; i++){
        if(!(process_pid_bitmap & BITMASK(i))){
            process_pid_bitmap |= BITMASK(i);
            return PROC_ALLOC_START + i * PROC_MEM_SIZE;
        }
    }
    return 0;
}

void deallocate_proc_mem(uint64_t addr){
        process_pid_bitmap &= ~BITMASK((addr - PROC_ALLOC_START) / PROC_MEM_SIZE);
}

void init_kernel_process(void) {
    kpcb = (struct ProcessControlBlock) {
        .ppid = 0,
        .pid = 0,
        .state = 0,

        .entry = (void*) KERNEL_VBASE - KERNEL_LOAD_ADDR,
        // .pfa = {}, // initialized in init_kernel_paging()->init_page_frame_allocator()
        // .ctx = {}, // initialized in init_kernel_paging()
        .stack = (void*) ((uint64_t) KERNEL_STACK_TOP),
        .heap = 0x0,

        .priority = 0,
        .cpu_context = 0
    };

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    init_kernel_paging(&(kpcb.pfa), MEMORY_SIZE_PAGES);
    printf("%s Kernel Paging\n", LOG_SYM_SUC);

    // Kernel Heap - Manage Kernel Dynamic Memory
    init_heap(kpcb, KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE, true);
    kpcb.heap = g_heap_malloc_state_base;
    printf("%s Heap: %p\n", LOG_SYM_SUC, kpcb.heap);

    // Map boot memory (bootloader set memory. i.e. GDT, TSS)
    map_memory_range(&kpcb, (void*)(PAGE_FRAME_ALLOCATOR_END + 1), (void*) (PAGE_FRAME_ALLOCATOR_END + PROC_SLOT_SIZE), (void*)(0x0));

    // Update GDTR (to use new virtual address)
    struct gdtr gdtr;
    asm volatile ("sgdt %0" : "=m"(gdtr));
    gdtr.base = PAGE_FRAME_ALLOCATOR_END + 1 + (gdtr.base);
    asm volatile("lgdt %0" : : "m"(gdtr));

    // Flush TSS - NOTICE! TSS address is HARDCODED in `gdt64.s` file
    flush_tss();
    printf("%s TSS\n", LOG_SYM_SUC);
}

PCB* alloc_proc(uint64_t ppid){
    cli();

    interrupts_ready = false;

    PCB* pcb = malloc(sizeof(PCB));

    *pcb = (struct ProcessControlBlock) {
        .pid = allocate_pid(),
        .ppid = ppid,
        .state = 0,
        .priority = 0,
        .cpu_context = 0,
        .real_mem_addr = allocate_proc_mem(),
        .pfa = kpcb.pfa,
        .ctx = {
            .old_pml4 = kpcb.ctx.pml4,
            .allocator = kpcb.ctx.allocator
        }
    };


    pcb->ctx.pml4 = allocate_page(pcb);

    map_memory_range(&kpcb, (void*)pcb->ctx.pml4, ((void*)pcb->ctx.pml4 + PAGE_SIZE - 1), (void*)pcb->ctx.pml4);
    map_memory_range(&kpcb, PROC_BIN_ADDR-PROC_STACK_SIZE, PROC_BIN_ADDR-PROC_STACK_SIZE+PROC_MEM_SIZE-1, pcb->real_mem_addr);

    // Change recursive paging to allow mapping of other pml4

    init_recursive_paging(pcb->ctx);

    kpcb.ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)(void*)pcb->ctx.pml4 | (uint64_t)PAGE_MAP_FLAGS;
    flush_tlb();

    map_memory_range(pcb, (void*) ((void*)pcb->ctx.pml4), (void*) ((void*)pcb->ctx.pml4 + PAGE_SIZE - 1), (void*)pcb->ctx.pml4);

    map_memory_range(pcb, PROC_BIN_ADDR-PROC_STACK_SIZE, PROC_BIN_ADDR-PROC_STACK_SIZE+PROC_MEM_SIZE-1, pcb->real_mem_addr);
    // Map Kernel (higher half)
    /* NOTE: Kernel is mapped to the same virtual address in both kernel PML4 and process PML4
             This ensures that the kernel in can see its own functions in the same address.
       NOTE: Kernel includes IDT (but not GDT!)
    */
    map_memory_range(pcb, (void*) (PROC_KERNEL_ADDR), (void*) (PAGE_FRAME_ALLOCATOR_END), (void*) (KERNEL_VBASE - KERNEL_LOAD_ADDR));

    // Map kernel boot (Kernel GDT and TSS)
    /* NOTE: For simplicity reasons, the GDT (initialized in Bootloader)
             stays in the same physical space, and thereby needs to be mapped
             to a different virtual address in the process's PML4 (GDT's address is ~0x8000,
             which is reserved for the process's code).
             GDT location is the same in both kernel and process PML4 (see ProcessMemoryLayout.md)
    */
    map_memory_range(pcb, (void*)(PAGE_FRAME_ALLOCATOR_END + 1), (void*) (PAGE_FRAME_ALLOCATOR_END + PROC_SLOT_SIZE), (void*)(0x0));


    pcb->heap = (void*) KERNEL_HEAP_START;

    map_memory_range(pcb, (void*) pcb->heap, pcb->heap + KERNEL_HEAP_SIZE_PAGES * 512 -1, pcb->heap);

    // Revert kernel pml4 change.
    kpcb.ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)kpcb.ctx.pml4 | (uint64_t)PAGE_MAP_FLAGS;
    // unmap_memory_range(&kpcb, PROC_BIN_ADDR-PROC_STACK_SIZE, PROC_BIN_ADDR-PROC_STACK_SIZE+PROC_MEM_SIZE-1, false);
    flush_tlb();

    pcb->stack = USER_LOAD_ADDR - 0x16;

    interrupts_ready = true;

    sti();

    return pcb;  
    

}
