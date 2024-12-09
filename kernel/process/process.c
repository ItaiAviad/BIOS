#include <process.h>

void init_kernel_process(void) {
    kpcb = (struct ProcessControlBlock) {
        .pid = 0,
        .state = 0,

        .entry = (void*) KERNEL_VBASE,
        // .pfa = {}, // initialized in init_kernel_paging()->init_page_frame_allocator()
        // .ctx = {}, // initialized in init_kernel_paging()
        .stack = (void*) ((uint64_t) KERNEL_STACK_TOP),
        .heap = 0x0,

        .priority = 0,
        .cpu_registers[0] = 0
    };

    // Initialize Kernel Paging:
    // Page Frame Allocator - Manage Physical Memory
    // Paging sturctures (PML4T, PDPT, PDT, PT)
    init_kernel_paging(&(kpcb.pfa), MEMORY_SIZE_PAGES);
    printf("%s Kernel Paging\n", LOG_SYM_SUC);

    // Kernel Heap - Manage Kernel Dynamic Memory
    init_heap(kpcb.ctx, KERNEL_HEAP_START, KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE);
    kpcb.heap = heap_malloc_state_base;
    printf("%s Heap: %p\n", LOG_SYM_SUC, kpcb.heap); 

    // Map boot memory (bootloader set memory. i.e. GDT, TSS)
    map_memory_range(kpcb.ctx, (void*)(PAGE_FRAME_ALLOCATOR_END + 1), (void*) (PAGE_FRAME_ALLOCATOR_END + PROC_SLOT_SIZE), (void*)(0x0));

    // Update GDTR (to use new virtual address)
    struct gdtr gdtr;
    asm volatile ("sgdt %0" : "=m"(gdtr));
    gdtr.base = PAGE_FRAME_ALLOCATOR_END + 1 + (gdtr.base);
    asm volatile("lgdt %0" : : "m"(gdtr));

    // Flush TSS - NOTICE! TSS address is HARDCODED in `gdt64.s` file
    flush_tss();
}
