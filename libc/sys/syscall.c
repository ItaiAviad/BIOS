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

void syscall_handler(pt_regs *regs) {
    long number = regs->rax;
    printf("rax: %x\n", regs->rax);
    printf("rdi: %x\n", regs->rdi);
    printf("rsi: %x\n", regs->rsi);
    printf("rdx: %x\n", regs->rdx);
    printf("r10: %x\n", regs->r10);
    printf("r8: %x\n", regs->r8);
    printf("r9: %x\n", regs->r9);
    printf("syscall number: %x\n", number);
}
#endif
