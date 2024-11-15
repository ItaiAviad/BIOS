// Libc wrappers for Kernel functions

#if defined(__is_libk)
#include <arch/x86_64/tty.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/mmu.h>
#endif

#include <types.h>
#include <kernel.h>
#include <sys/syscall.h>

void shutdown(void) {
#if defined(__is_libk)
    qemu_shutdown();
#else
    syscall(sys_shutdown);
#endif
}

void tty_init(void) {
#if defined(__is_libk)
    terminal_initialize();
#else
    syscall(sys_tty_init);
#endif
}

void ursp(uint64_t rsp) {
#if defined(__is_libk)
    map_memory_range(k_ctx, (void*) (rsp - MB), (void*) rsp, (void*) (rsp - MB));
    set_rsp(rsp);
#else
    syscall(sys_ursp, rsp);
#endif
}
