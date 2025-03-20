// Kernel Main File

// libc
#include <memory.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// arch/x86_64

#include <process.h>


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
    PCB* pcb = alloc_proc(kpcb.ppid);

    void* elf_bin = readelf((void*)USER_LOAD_ADDR, "/mnt/mount1/user_prog", false);

    set_pml4_address((uint64_t *) pcb->ctx.pml4);

    jump_usermode((void*)USER_LOAD_ADDR, (void*)(pcb->stack));
    while (1) {}
}
