// Libc wrappers for Kernel functions

#if defined(__is_libk)
#include <arch/x86_64/tty.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/mmu.h>
#include <arch/x86_64/io.h>
#else
#include <shell.h>
#endif

#include <types.h>
#include <kernel.h>
#include <sys/syscall.h>

void keycode_ext_handler(__attribute__((unused)) int c) {
#if !defined(__is_libk)
    if (c == CURSOR_UP) cuph();
    else if (c == CURSOR_LEFT) clefth();
    else if (c == CURSOR_RIGHT) crighth();
    else if (c == CURSOR_DOWN) cdownh();
#endif
}

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

void stdin_clear() {
#if defined(__is_libk)
    for (int i = 0; i < buffer_len(); ++i) {
        buffer_put_c('\b');
    }
#else
    syscall(sys_stdin_clear);
#endif
}

void stdin_insert(const char* s) {
#if defined(__is_libk)
    for (int i = 0; i < (int)strlen(s); ++i) {
        buffer_put_c(s[i]);
    }
#else
    syscall(sys_stdin_insert, s);
#endif
}
