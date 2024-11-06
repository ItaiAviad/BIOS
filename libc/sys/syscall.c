#include <sys/syscall.h>

#if defined(__is_libk)
void enable_syscall() {
    uint64_t efer = read_msr(MSR_EFER);
    efer |= EFER_SCE; // Set the SCE bit to enable syscall/sysret

    write_msr(MSR_EFER, efer);
}

void configure_segments() {
    // Set kernel CS to 0x08 and user CS to 0x1b (these are typical values)
    uint64_t star_value = ((uint64_t)0x08 << 32) | ((uint64_t)0x18 << 48);
    write_msr(MSR_STAR, star_value);
}

void init_syscall() {
    // Enable syscall/sysret using EFER.SCE
    enable_syscall();

    // Set up the syscall entry point
    write_msr(MSR_LSTAR, (uint64_t)syscall_entry);

    configure_segments();
}

void syscall_handler(struct pt_regs *regs) {
    __attribute__((unused)) uint64_t syscall_number = regs->rax;
}
#endif
