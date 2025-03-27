#include <sys/syscall.h>
#include <sched.h>

#if defined(__is_libk)
void enable_syscall() {
    uint64_t efer = read_msr(MSR_EFER);
    uint32_t efer_lo = efer & 0xffffffff;
    uint32_t efer_hi = efer >> 32;
    efer_lo |= EFER_SCE; // Set the SCE bit to enable syscall/sysret

    write_msr(MSR_EFER, efer_lo, efer_hi);
}

void configure_segments(uint16_t kernel_cs, uint16_t kernel_ss, uint16_t user_cs, uint16_t user_ss) {
    // Pack the segment selectors into the MSR_STAR value
    uint64_t star_value = 
        ((uint64_t)kernel_ss << 48) |   // Kernel SS goes in the upper part
        ((uint64_t)kernel_cs << 32) |   // Kernel CS goes just below SS
        ((uint64_t)user_ss << 16) |     // User SS
        (uint64_t)user_cs;              // User CS goes in the lowest part
    
    // Write MSR_STAR
    write_msr(MSR_STAR, (uint32_t)(star_value & 0xFFFFFFFF), (uint32_t)(star_value >> 32));
}

struct kernel_gs_base kgb = {
    .kstack = KERNEL_STACK_TOP,
    .ustack = 0x0
};

void init_syscall() {
    // Enable syscall/sysret using EFER.SCE
    enable_syscall();

    // Set up the syscall entry point
    write_msr(MSR_LSTAR, ((uint64_t)syscall_entry & 0xffffffff), ((uint64_t)syscall_entry >> 32));

    // Set up KernelGSBase
    write_msr(MSR_KERNEL_GS_BASE, ((uint64_t)&kgb & 0xffffffff), ((uint64_t)&kgb >> 32));

    configure_segments(KERNEL_CS_SELECTOR_OFFSET_GDT, KERNEL_SS_SELECTOR_OFFSET_GDT, USER_CODE_DESCRIPTOR_OFFSET, USER_DATA_DESCRIPTOR_OFFSET);
}

int64_t syscall_handler(cpu_state *regs) {
    long number = regs->rax;

    // printf("syscall number: %p\n", number);
    // printf("rdi: %p\n", regs->rdi);
    // printf("rsi: %p\n", regs->rsi);
    // printf("rdx: %p\n", regs->rdx);
    // printf("r10: %p\n", regs->r10);
    // printf("rcx: %p\n", regs->rcx);
    // // printf("r11: %p\n", regs->r11);
    // // printf("r12: %p\n", regs->r12);
    // // printf("r13: %p\n", regs->r13);
    // // printf("r14: %p\n", regs->r14);
    // // printf("r15: %p\n", regs->r15);
    // printf("rbx: %p\n", regs->rbx);
    // // printf("rsp: %p\n", regs->rsp);
    // // printf("rbp: %p\n", regs->rbp);
    // printf("r8: %p\n", regs->r8);
    // printf("r9: %p\n", regs->r9);
    // int x = 1/0;

    // Virtual Address Space (see `syscall_entry` in `syscall_s.s` for stack switch)
    // Save current VAS
    uint64_t prev_cr3;
    __asm__ volatile (
        "mov %0, cr3\n"
        : "=r"(prev_cr3) // Output
    );
    // Switch to Kernel Virtual Address Space
    if (prev_cr3 != (uint64_t) (PML4_KERNEL)) {
        flush_tlb();
        invlpg((uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM,PML4_RECURSIVE_ENTRY_NUM));
        set_pml4_address((uint64_t*) (PML4_KERNEL));
    }

    int SYS_T_LEN = sizeof(SYSCALL_TABLE) / sizeof(SYSCALL_TABLE[0]);
    if (number >= SYS_T_LEN || SYSCALL_TABLE[number] == NULL) {
        return -1;
    }
    memcpy(&(current_pcb->cpu_context), regs, sizeof(cpu_state));

    int64_t ret = SYSCALL_TABLE[number](regs->rdi, regs->rsi, regs->rdx, regs->r10, regs->r8, regs->r9, regs);
    // Switch back to Kernel PML4
    flush_tlb();
    invlpg((uint64_t*)get_addr_from_table_indexes(PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM, PML4_RECURSIVE_ENTRY_NUM,PML4_RECURSIVE_ENTRY_NUM));
    set_pml4_address((uint64_t*) (prev_cr3));
    return ret;
}

#endif
