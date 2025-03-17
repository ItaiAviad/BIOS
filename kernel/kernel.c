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
// network
#include <net/if.h>
#include <net/rtl8139.h>
#include <vfs.h>

extern void jump_usermode(void* entry, void* sp);
void user_init();

int interrupts_ready;

int kmain(void) {

    interrupts_ready = false;
    // TTY - Terminal
    cli();
    // ISR - Interrupt Service Routines
    init_isr_handlers();
    // printf("%s ISRs\n", LOG_SYM_SUC);

    // PIC - Programmable Interrupt Controller
    // IMPORTANT: PIC should be initialized at the end of Kernel's initializations
    // to avoid race conditions!
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    // printf("%s PIC\n", LOG_SYM_SUC);

    // Initialize Kernel Process (Paging, Stack, Heap, etc.)
    init_kernel_process();

    interrupts_ready = true;

    sti();

    terminal_initialize();
    printf("%s Terminal\n", LOG_SYM_SUC);

    // Init PCI
    enumerate_pci();

    printf("%s PCI\n", LOG_SYM_SUC);
    print_pci_devices();

    // Setup AHCI and enumerate Disks
    enumerate_disks();

    // Init Syscall
    init_syscall();

    init_vfs();

    // while (1){

    // }

    srand(time());

    rtl8139_init();

    // usermode

    vfs_mkdir("/mnt");
    vfs_mkdir("/mnt/mount1");
    mount_file_system("/mnt/mount1", 0, EXT2_START_OFFSET,FILESYSTEM_TYPE_EXT2);

    linkedListNode* list = list_dir("/mnt/mount1"); // The test

    while(list){
        printf("%s\n",list->data);
        list = list->next;
    }
    user_init();

    while (1) {}

    return 0;
}

void user_init() {
    interrupts_ready = false;
    cli();

    struct ProcessControlBlock pcb = {
        .pid = 0,
        .state = 0,


        .entry = (void*) USER_LOAD_ADDR,
        .ctx = {
            .start_addr = (uint64_t) pcb.entry,
            .kernel_addr = (uint64_t) pcb.entry + PROC_KERNEL_ADDR,
            .memory_size_pages = pcb.pfa.num_pages,
            .allocator = kpcb.ctx.allocator,
            .old_pml4 = (uint64_t*) PML4_KERNEL,
            .pml4 = pcb.entry + PROC_PML4T_ADDR
        },

        .stack = 0,
        .heap = 0,

        .priority = 0,
        .cpu_context = 0
    };

    // Map process memory in kernel's PML4T
    map_memory_range(kpcb.ctx, (void*) (pcb.entry + PROC_BIN_ADDR), (void*) (pcb.entry + PROC_MEM_SIZE - PROC_BIN_ADDR), (void*) (pcb.entry + PROC_BIN_ADDR));
    // Map process memory in kernel's PML4T (identity map for i.e. heap addresses)
    map_memory_range(kpcb.ctx, (void*) (PROC_BIN_ADDR), (void*) (PROC_MEM_SIZE / 2 - 1), (void*) (pcb.entry + PROC_BIN_ADDR));

    // Read binary into process memory
    // read_disk(0, 0, PROC_BIN_SIZE, (void*) (PROC_BIN_ADDR));

    void* elf_bin = readelf((void*) PROC_BIN_ADDR, "/mnt/mount1/user_prog", false);
    if (!elf_bin) {
        printf("Error reading elf binary\n");
        while (1) {}
    }

    // PML4T
    kpcb.ctx.pml4[PML4_RECURSIVE_ENTRY_NUM] = (uint64_t)pcb.ctx.pml4 | (uint64_t)PAGE_MAP_FLAGS;
    init_recursive_paging(pcb.ctx);
    
    // Mark process binary, pfa as allocated
    memset(pcb.ctx.allocator->bitmap, 0x1, UPPER_DIVIDE(PROC_PML4T_ADDR, PAGE_SIZE));

    // Map process binary, pfa
    // map_memory_range(pcb.ctx, (void*) (PROC_BIN_ADDR), (void*) (PROC_PML4T_ADDR - 1), (void*) (pcb.entry + PROC_BIN_ADDR));
    map_memory_range(pcb.ctx, (void*) (PROC_BIN_ADDR), (void*) ((PROC_MEM_SIZE / 2 - 1)), (void*) (pcb.entry + PROC_BIN_ADDR));
    // Map process PML4T
    // map_memory_range(pcb.ctx, (void*) (PROC_PML4T_ADDR), (void*) (PROC_PML4T_ADDR + PAGE_SIZE - 1), (void*)pcb.ctx.pml4);

    // Stack - Map Stack pages in process's VAS
    // ASLR stack
    // NOTICE! Stack address after this chunk of code is relative to the **process's VAS**
    uint64_t stack_slot = rand() % (PROC_SLOTS) + (PROC_SLOTS_OFFSET);
    while (stack_slot < PROC_SLOTS_OFFSET)
        stack_slot = rand() % (PROC_SLOTS) + (PROC_SLOTS_OFFSET);
    pcb.stack = (void*) (stack_slot * PROC_SLOT_SIZE);
    // map_memory_range(pcb.ctx, (void*) (pcb.stack), (void*) (pcb.stack + PROC_STACK_SIZE - 1), (void*) ((uint64_t) pcb.entry + (uint64_t) pcb.stack));
    pcb.stack += PROC_STACK_SIZE - 0x100;

    // Heap
    // ASLR heap
    // NOTICE! Heap address after this chunk of code is relative to the **process's VAS**
    uint64_t heap_slot = rand() % (PROC_SLOTS) + (PROC_SLOTS_OFFSET);
    while (heap_slot < PROC_SLOTS_OFFSET || abs(stack_slot - heap_slot) <= 1)
        heap_slot = rand() % (PROC_SLOTS) + (PROC_SLOTS_OFFSET);
    pcb.heap = (void*) (heap_slot * PROC_SLOT_SIZE);
    // TODO: Update g_heap_malloc_state_base so kernel mallocs in process's heap
    // map_memory_range(pcb.ctx, (void*) (pcb.heap), (void*) (pcb.heap + PROC_STACK_SIZE - 1), (void*) ((uint64_t) pcb.entry + (uint64_t) pcb.heap));
    // pcb.heap += (uint64_t) pcb.entry; // if heap relative to kernel's VAS
    // Init heap
    init_heap(pcb.ctx, (uint64_t) pcb.heap, PROC_HEAP_SIZE, false);

    // printf("stack: %d, heap: %d\n", stack_slot, heap_slot);
    // printf("stack: %p, heap: %p\n", pcb.stack, pcb.heap);

    // Map Kernel (higher half)
    /* NOTE: Kernel is mapped to the same virtual address in both kernel PML4 and process PML4
             This ensures that the kernel in can see its own functions in the same address.
       NOTE: Kernel includes IDT (but not GDT!)
    */
    map_memory_range(pcb.ctx, (void*) (PROC_KERNEL_ADDR), (void*) (PAGE_FRAME_ALLOCATOR_END), (void*) (KERNEL_VBASE - KERNEL_LOAD_ADDR));

    // Map kernel boot (Kernel GDT and TSS)
    /* NOTE: For simplicity reasons, the GDT (initialized in Bootloader)
             stays in the same physical space, and thereby needs to be mapped
             to a different virtual address in the process's PML4 (GDT's address is ~0x8000,
             which is reserved for the process's code).
             GDT location is the same in both kernel and process PML4 (see ProcessMemoryLayout.md)
    */
    map_memory_range(pcb.ctx, (void*)(PAGE_FRAME_ALLOCATOR_END + 1), (void*) (PAGE_FRAME_ALLOCATOR_END + PROC_SLOT_SIZE), (void*)(0x0));

    // Allow later kernel maps
    init_recursive_paging(kpcb.ctx);

    // Switch PML4 to use the (new) PML4
    flush_tlb();
    invlpg((uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM,PML4_RECURSIVE_ENTRY_NUM));
    set_pml4_address((uint64_t *) pcb.ctx.pml4);

    // printf("123\n");
    interrupts_ready = true;
    sti();
    jump_usermode((void*)PROC_BIN_ADDR, (void*)(pcb.stack));
    while (1) {}
}
