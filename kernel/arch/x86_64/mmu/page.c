#include <arch/x86_64/mlayout.h>
#include <math.h>
#include <arch/x86_64/mmu.h>
#include <process.h>
#include <string.h>

void init_kernel_paging(PageFrameAllocator* allocator, size_t memory_size_pages) {
    init_page_frame_allocator(allocator, memory_size_pages);

    cli();
    Context boot_ctx = {
        .start_addr = 0x0,
        .kernel_addr = 0x0,
        .memory_size_pages = memory_size_pages,
        .allocator = allocator,
        .old_pml4 = (uint64_t*) PML4_BOOT,
        .pml4 = (uint64_t*) PML4_BOOT
    };

    kpcb.ctx.start_addr = 0x0;
    kpcb.ctx.kernel_addr = 0x0;
    kpcb.ctx.memory_size_pages = memory_size_pages;
    kpcb.ctx.allocator = allocator;
    kpcb.ctx.old_pml4 = (uint64_t*) PML4_BOOT;
    kpcb.ctx.pml4 = (uint64_t*) PML4_KERNEL;

    allocator->bitmap[(uint64_t)boot_ctx.pml4 / PAGE_SIZE] = 1; // Mark pml4 boot as allocated

    allocator->bitmap[(uint64_t)(boot_ctx.pml4+PAGE_SIZE) / PAGE_SIZE] = 1; // Mark pdpt boot as allocated

    // invlpg(kpcb.ctx.pml4);

    memset(kpcb.ctx.pml4, 0, PAGE_SIZE);

    boot_ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)PML4_KERNEL | (uint64_t)PAGE_MAP_FLAGS;

    init_recursive_paging(kpcb.ctx);

    // flush_tlb();

    // invlpg((uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM,PML4_RECURSIVE_ENTRY_NUM));

    switch_context(&kpcb);

    cli();

    // We can reuse the pages after switching to the kernel's pml4

    allocator->bitmap[(uint64_t)boot_ctx.pml4 / PAGE_SIZE] = 0; // Mark pml4 boot as not allocated

    allocator->bitmap[(uint64_t)(boot_ctx.pml4+PAGE_SIZE) / PAGE_SIZE] = 0; // Mark pdpt boot as not allocated

    #ifdef DEBUG
    printf("%s PML4=%d, PML4[0]=%d, PML4[0][0]=%d, bitmap[PML4/PAGE_SIZE]=%d\n", DEBUG, PML4_KERNEL, ((uint64_t*) PML4_KERNEL)[0], allocator->bitmap[PML4_KERNEL/PAGE_SIZE]);
    #endif
}


uint64_t* get_addr_from_table_indexes(uint16_t pml4_index, uint16_t pdpt_index, uint16_t pd_index, uint16_t pt_index) {

    // Each index contributes 9 bits to the virtual address
    uint64_t virtual_address = 0;

    // Shift and add each index
    virtual_address |= ((uint64_t)(pml4_index & 0b111111111) << 39); // PML4 index (bits 39-47)
    virtual_address |= ((uint64_t)(pdpt_index & 0b111111111) << 30); // PDPT index (bits 30-38)
    virtual_address |= ((uint64_t)(pd_index & 0b111111111) << 21);   // PD index (bits 21-29)
    virtual_address |= ((uint64_t)(pt_index & 0b111111111) << 12);   // PT index (bits 12-20)

    // Now sign-extend the 48th bit to ensure canonical addressing
    if (virtual_address & (1ULL << 47)) {
        // Sign extend by setting all bits from 48-63 if bit 47 is 1 (kernel space)
        virtual_address |= 0xFFFF000000000000;
    }

    return (uint64_t*) virtual_address; // Return the final 64-bit virtual address
}

/**
* @brief Initialize the PML4 recursive mapping technique.
*        Maps the recursive entry in the PML4 to the PML4 itself.
* @param ctx The current paging context that contains the PML4.
*
* Note: Ensure the PML4 is already mapped before calling this function (And should be identity mapped).
*/
void init_recursive_paging(Context ctx){
    ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)ctx.pml4 | PAGE_MAP_FLAGS;
}

void switch_context(PCB* pcb) {
    cli();

    // Map New PML4
    // Map the Page of the NEW PML4T in the OLD PML4T
    // map_page((uint64_t *) ctx.old_pml4, &ctx.allocator, (uint64_t)ctx.pml4, (uint64_t)ctx.pml4, PAGE_MAP_FLAGS);
    // Map self (new) PML4 Page (PML4 Table Page) (IMPORTANT! Not doing so causes page fault on PML4 Table access (not good  ¯\_(ツ)_/¯))
    // map_page(ctx, (uint64_t)ctx.pml4, (uint64_t)ctx.pml4, PAGE_MAP_FLAGS);
    
    // Map Kernel (In new PML4)
    // Map Kernel + Page Frame Allocator + Pagign Tables in new Kernel Context
    memset(pcb->ctx.allocator->bitmap, 1, UPPER_DIVIDE(PAGE_FRAME_ALLOCATOR_END, PAGE_SIZE));

    // map_memory_range(ctx, (void*)MBR_LOAD_ADDR, (void*) (MBR_LOAD_ADDR + (1 * MB)), (void*)MBR_LOAD_ADDR);
    map_memory_range(pcb, (void*)0x0, (void*) (0x0 + (3 * MB) - 1), (void*)0x0);
    map_memory_range(pcb, (void*)(KERNEL_VBASE - KERNEL_LOAD_ADDR), (void*) PAGE_FRAME_ALLOCATOR_END - 1, (void*)(KERNEL_VBASE - KERNEL_LOAD_ADDR));
    map_memory_range(pcb, (void*)pcb->ctx.pml4, pcb->ctx.pml4+PAGE_SIZE-1, (void*)pcb->ctx.pml4);
    // Switch PML4 to use the (new) s PML4
    cli();
    set_pml4_address((uint64_t *) pcb->ctx.pml4);
    sti();
}

void set_pml4_address(uint64_t* pml4) {
    __asm__ volatile (
        "mov %%rax ,%0\n"  // Load the PML4 table address into RAX
        "mov %%cr3 ,%%rax\n" // Load the address from RAX into CR3
        : 
        : "r" (pml4) // Input operand: the address of the PML4 table
        : "rax" // Clobbered register
    );
}

void invlpg(void* addr) {
    __asm__ volatile (
        "invlpg [%0]" 
    ::"r"(addr) : "memory", "rax");
}

void flush_tlb() {
    set_pml4_address(get_pml4_address());
}

uint64_t* get_pml4_address() {
    uint64_t pml4_address;

    // Use inline assembly to read CR3
    asm volatile (
        "mov %0, %%cr3"  // Move the value of CR3 into pml4_address
        : "=r" (pml4_address)  // Output operand
        :
        :
    );

    return (uint64_t*) pml4_address;  // Return the address as a pointer
}


void map_page(PCB* pcb, void* virtual_address, void* physical_address, uint64_t flags) {

    interrupts_ready = false;
    cli();

    // Calculate indices
    uint64_t pml4_index = ((uint64_t)virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t)virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = ((uint64_t)virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t)virtual_address >> 12) & 0x1FF;

    uint64_t* pml4_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM);
    invlpg(pml4_recursive);
    uint64_t *pdpt = NULL, *pd = NULL, *pt = NULL;

    // Ensure PDPT, PD, and PT entries exist (allocate and zero if necessary)
    uint64_t* pdpt_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, pml4_index);
    if (!(pml4_recursive[pml4_index] & PAGE_PRESENT)) { // Check if PDPT entry exists (in PML4T)
        pdpt = allocate_page(pcb);
        if(pdpt == NULL){
            printf("Error, couldn't get pdpt from allocator\n");
            return;
        }
        pml4_recursive[pml4_index] = (uint64_t)pdpt | (uint64_t)PAGE_MAP_FLAGS;
        invlpg(pdpt_recursive);
        memset(pdpt_recursive, 0, PAGE_SIZE);
    } else {
        pdpt = (uint64_t*) (pml4_recursive[pml4_index] & ~0xFFF);
    }
    uint64_t* pd_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, pml4_index, pdpt_index);
    if (!(pdpt_recursive[pdpt_index] & PAGE_PRESENT)) { // Check if PDT entry exists (in PDPT)
        pd = allocate_page(pcb);
        if(pd == NULL){
            printf("Error, couldn't get pd from allocator\n");
            return;
        }
        pdpt_recursive[pdpt_index] = (uint64_t)pd | (uint64_t)PAGE_MAP_FLAGS;
        invlpg(pd_recursive);
        memset(pd_recursive, 0, PAGE_SIZE);
    } else {
        pd = (uint64_t*) (pdpt_recursive[pdpt_index] & ~0xFFF);
    }
    uint64_t* pt_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, pml4_index, pdpt_index, pd_index);
    if (!(pd_recursive[pd_index] & PAGE_PRESENT)) { // Check if PT entry exists (in PDT)
        pt = allocate_page(pcb);
        if(pt == NULL){
            printf("Error, couldn't get pd from allocator\n");
            return;
        }
        pd_recursive[pd_index] = (uint64_t)pt | (uint64_t)PAGE_MAP_FLAGS;
        invlpg(pt_recursive);
        memset(pt_recursive, 0, PAGE_SIZE);
    } else {
        pt = (uint64_t*) (pd_recursive[pd_index] & ~0xFFF);
    }
    invlpg(pt_recursive);

    // Map the physical address to the virtual address in the page table
    pt_recursive[pt_index] = (uint64_t) physical_address | flags;

    interrupts_ready = true;
    sti();
}

void unmap_page(uint64_t virtual_address) {
    interrupts_ready = false;
    cli();

    uint64_t pml4_index = ((uint64_t)virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t)virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = ((uint64_t)virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t)virtual_address >> 12) & 0x1FF;

    uint64_t* pml4_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM);
    uint64_t* pdpt_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, pml4_index);
    uint64_t* pd_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, pml4_index, pdpt_index);
    uint64_t* pt_recursive = (uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, pml4_index, pdpt_index, pd_index);

    if (!(pml4_recursive[pml4_index] & PAGE_PRESENT)) return; // PDPT not present
    if (!(pdpt_recursive[pdpt_index] & PAGE_PRESENT)) return; // PD not present
    if (!(pd_recursive[pd_index] & PAGE_PRESENT)) return; // PT not present
    if (!(pt_recursive[pt_index] & PAGE_PRESENT)) return; // Page not mapped

    // Clear the page table entry
    pt_recursive[pt_index] = 0;
    invlpg(virtual_address);

    interrupts_ready = true;
    sti();
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
    printf("%s pml4: %d pdpt: %d pd: %d pt: %d pd_index: %d pd[pd_index]: %d pt_index: %d pt[pt_index]: %d\n", DEBUG, pml4, pdpt, pd, pt, pd_index, pd[pd_index], pt_index, pt[pt_index]);
    #endif

    return (int64_t*) pt[pt_index];
}

PCB* find_pcb_by_pid(uint64_t pid){
    linkedListNode* curr = pcb_list;
    while(curr){
        if(((PCB*)curr->data)->pid == pid){
            return (PCB*)curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}
