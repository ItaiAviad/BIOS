#include <sys/syscall.h>

#if defined(__is_libk)
void enable_syscall() {
    uint64_t efer = read_msr(MSR_EFER);
    uint32_t efer_lo = efer & 0xffffffff;
    uint32_t efer_hi = efer >> 32;
    efer_lo |= EFER_SCE; // Set the SCE bit to enable syscall/sysret

    write_msr(MSR_EFER, efer_lo, efer_hi);
}

void configure_segments() {
    // Set kernel CS to 0x08 and user CS to 0x1b (these are typical values)
    uint64_t star_value = ((uint64_t)0x08 << 32) | ((uint64_t)0x18 << 48);
    write_msr(MSR_STAR, star_value & 0xffffffff, (star_value >> 32));
}

void init_syscall() {
    // Enable syscall/sysret using EFER.SCE
    enable_syscall();

    // Set up the syscall entry point
    write_msr(MSR_LSTAR, ((uint64_t)syscall_entry & 0xffffffff), ((uint64_t)syscall_entry >> 32));

    configure_segments();
}

int64_t syscall_handler(pt_regs *regs) {
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
    
    int SYS_T_LEN = sizeof(SYSCALL_TABLE) / sizeof(SYSCALL_TABLE[0]);
    if (number >= SYS_T_LEN || SYSCALL_TABLE[number] == NULL) {
        return -1;
    }

    return SYSCALL_TABLE[number](regs->rdi, regs->rsi, regs->rdx, regs->r10, regs->r8, regs->r9); 
}

#endif
