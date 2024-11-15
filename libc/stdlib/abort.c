#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
	// TODO: Add proper kernel panic.
	printf("kernel: panic: abort()\n");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
    syscall(sys_abort);
#endif
	while (1) {}
	__builtin_unreachable();
}
