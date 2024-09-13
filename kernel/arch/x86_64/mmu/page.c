#include <arch/x86_64/mmu.h>

void init_kernel_paging(PageFrameAllocator* allocator, size_t memory_size_pages) {
    init_page_frame_allocator(allocator, memory_size_pages);

    Context boot_ctx = {
        .start_addr = 0x0,
        .kernel_start_offset = 0x0,
        .memory_size_pages = memory_size_pages,
        .allocator = *allocator,
        .old_pml4 = (uint64_t*) PML4_BOOT,
        .pml4 = (uint64_t*) PML4_BOOT
    };
    // Map important pages on Bootloader's PML4 Section (Kernel and Page Frame Allocator)
    map_memory_range(boot_ctx, 0x0, PAGE_FRAME_ALLOCATOR_END, 0x0);

    k_ctx.start_addr = 0x0;
    k_ctx.kernel_start_offset = 0x0;
    k_ctx.memory_size_pages = memory_size_pages;
    k_ctx.allocator = *allocator;
    k_ctx.old_pml4 = (uint64_t*) PML4_BOOT;
    k_ctx.pml4 = (uint64_t*) (k_ctx.start_addr + k_ctx.kernel_start_offset + PML4_KERNEL);

    switch_context(k_ctx);

    #ifdef DEBUG
    printf("%s PML4=%x, PML4[0]=%x, PML4[0][0]=%x, bitmap[PML4/PAGE_SIZE]=%x\n", DEBUG, PML4_KERNEL, ((uint64_t*) PML4_KERNEL)[0], allocator->bitmap[PML4_KERNEL/PAGE_SIZE]);
    #endif
}

void switch_context(Context ctx) {
    cli();
    // Initialize (new) Page Frame Allocator
    init_page_frame_allocator(&ctx.allocator, ctx.memory_size_pages);

    // Map New PML4
    // Map the Page of the NEW PML4T in the OLD PML4T
    // map_page((uint64_t *) ctx.old_pml4, &ctx.allocator, (uint64_t)ctx.pml4, (uint64_t)ctx.pml4, PAGE_MAP_FLAGS);
    // Map self (new) PML4 Page (PML4 Table Page) (IMPORTANT! Not doing so causes page fault on PML4 Table access (not good  ¯\_(ツ)_/¯))
    // map_page(ctx, (uint64_t)ctx.pml4, (uint64_t)ctx.pml4, PAGE_MAP_FLAGS);
    
    // Map Reserved Paging Tables
    map_reserved_paging_tables(ctx);
    
    // Map Kernel (In new PML4)
    // Map Kernel + Page Frame Allocator + Pagign Tables in new Kernel Context
    map_memory_range(ctx, ctx.start_addr + ctx.kernel_start_offset, ctx.start_addr + ctx.kernel_start_offset + PAGE_FRAME_ALLOCATOR_END, 0x0);
    map_memory_range(ctx, 0x4000000, 0x4200000, 0x0);
    
    // Switch PML4 to use the (new) s PML4
    set_pml4_address((uint64_t *) ctx.pml4);
    sti();
}

void set_pml4_address(uint64_t* pml4){
    __asm__ volatile (
        "cli\n"
        "mov %%rax, %0\n"     // Move the address of the PML4 table into the RAX register
        "mov %%cr3, %%rax\n"  // Move the address from RAX into the CR3 register
        "mov %%rax, %%cr3\n"
        "sti"
        : 
        : "r" (pml4) // Input operand: the address of the PML4 table
        : "rax" // Clobbered register
    );
}

void map_page(Context ctx, uint64_t virtual_address, uint64_t physical_address, uint64_t flags) {
    // Calculate indices
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_address >> 12) & 0x1FF;

    uint64_t *pdpt = NULL, *pd = NULL, *pt = NULL;

    (&ctx.allocator)->bitmap[(uint64_t)ctx.pml4/PAGE_SIZE] = 1; // Map page managment structure as in use

    // Ensure PDPT, PD, and PT entries exist (allocate and zero if necessary)
    if (!(ctx.pml4[pml4_index] & PAGE_PRESENT)) { // Check if PDPT entry exists (in PML4T)
        pdpt = allocate_and_zero_page(ctx, true);
        ctx.pml4[pml4_index] = ((uint64_t) pdpt) | PAGE_MAP_FLAGS;
        
        // Map new self
        map_page(ctx, (uint64_t)ctx.pml4[pml4_index], (uint64_t)ctx.pml4[pml4_index], PAGE_MAP_FLAGS);
    } else {
        pdpt = (uint64_t*) (ctx.pml4[0] & ~0xFFF);
        (&ctx.allocator)->bitmap[(uint64_t)pdpt/PAGE_SIZE] = 1;
    }
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) { // Check if PDT entry exists (in PDPT)
        pd = allocate_and_zero_page(ctx, true);
        pdpt[pdpt_index] = ((uint64_t) pd) | PAGE_MAP_FLAGS;
        
        // Map new self
        map_page(ctx, (uint64_t)pdpt[pdpt_index], (uint64_t)pdpt[pdpt_index], PAGE_MAP_FLAGS);
    } else {
        pd = (uint64_t*) (pdpt[pdpt_index] & ~0xFFF);
        (&ctx.allocator)->bitmap[(uint64_t)pd/PAGE_SIZE] = 1;
    }
    if (!(pd[pd_index] & PAGE_PRESENT)) { // Check if PT entry exists (in PDT)
        pt = allocate_and_zero_page(ctx, true);
        pd[pd_index] = ((uint64_t) pt) | PAGE_MAP_FLAGS;

        // Map new self
        map_page(ctx, (uint64_t)pdpt[pdpt_index], (uint64_t)pdpt[pdpt_index], PAGE_MAP_FLAGS);
        memset(pt, 0, PAGE_SIZE);
    } else {
        pt = (uint64_t*) (pd[pd_index] & ~0xFFF);
        (&ctx.allocator)->bitmap[(uint64_t)pt/PAGE_SIZE] = 1;
    }

    // Map the physical address to the virtual address in the page table
    pt[pt_index] = physical_address | flags;
}

void unmap_page(Context ctx, uint64_t virtual_address) {
    // Calculate PT index
    uint64_t pt_index = (virtual_address >> 12) & 0x1FF;
    int64_t *pt = NULL;

    pt = is_page_mapped(ctx.pml4, virtual_address);
    if (pt < (int64_t*) 0x0)
        return;
    
    // Unmap the physical address to the virtual address in the page table
    pt[pt_index] = 0x0;
}

int64_t* is_page_mapped(uint64_t* pml4, uint64_t virtual_address){
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_address >> 12) & 0x1FF;

    uint64_t *pdpt = NULL, *pd = NULL, *pt = NULL;

    // Check if all page directories/tables to address exist in the provided pml4
    if (!(pml4[pml4_index] & PAGE_PRESENT)) {
        return (int64_t*) -0x1;
    }
    pdpt = (uint64_t*) (pml4[pml4_index] & ~0xFFF);

    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        return (int64_t*) -0x1;
    }
    pd = (uint64_t*) (pdpt[pdpt_index ] & ~0xFFF);

    if (!(pd[pd_index] & PAGE_PRESENT)) {
        return (int64_t*) -0x1;
    }
    pt = (uint64_t*) (pd[pd_index] & ~0xFFF);

    if(!(pt[pt_index] & PAGE_PRESENT)){
        return (int64_t*) -0x1;
    }
    #ifdef DEBUG
    printf("%s pml4: %x pdpt: %x pd: %x pt: %x pd_index: %x pd[pd_index]: %x pt_index: %x pt[pt_index]: %x\n", DEBUG, pml4, pdpt, pd, pt, pd_index, pd[pd_index], pt_index, pt[pt_index]);
    #endif

    return (int64_t*) pt;
}
