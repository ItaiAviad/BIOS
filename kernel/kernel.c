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

#include <sched.h>
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

#include <proc.h>

extern void jump_usermode(cpu_state*);
void user_init();

int interrupts_ready;

static inline uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile (
        "rdmsr" 
        : "=a"(low), "=d"(high) 
        : "c"(msr)
    );
    return ((uint64_t)high << 32) | low;
}

static inline void write_msr(uint32_t msr, uint32_t lo, uint32_t hi) {
    __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}


void dump_msr_star(void) {
    uint64_t star = read_msr(MSR_STAR);
    uint16_t kernel_ss = (star >> 48) & 0xFFFF;
    uint16_t kernel_cs = (star >> 32) & 0xFFFF;
    uint16_t user_ss = (star >> 16) & 0xFFFF;
    uint16_t user_cs = star & 0xFFFF;

    printf("MSR_STAR: 0x%x\n", star);
    printf("  Kernel SS: 0x%x\n", kernel_ss);
    printf("  Kernel CS: 0x%x\n", kernel_cs);
    printf("  User SS base: 0x%x (sysret SS: 0x%x)\n", user_ss, user_ss + 8);
    printf("  User CS base: 0x%x (sysret CS: 0x%x)\n", user_cs, user_cs + 8);
}

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

    printf("%x", get_tss_addr());


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

    dump_msr_star();
    // user_init();
    PCB* pcb = alloc_proc();
    pcb->ppid = kpcb.pid;
    load_proc_mem(pcb, "/mnt/mount1/user_prog");
    // run_proc(pcb);

    while (1) {}

    return 0;
}

// void user_init() {
//     PCB* pcb = alloc_proc();
//     pcb->ppid = kpcb.pid;
//     load_proc_mem(pcb, "/mnt/mount1/user_prog");
//     run_proc(pcb);
// }
