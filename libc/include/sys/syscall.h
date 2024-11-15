// Syscalls

#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <kernel.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082

#define EFER_SCE (1 << 0)  // SCE bit is the lowest bit in MSR_EFER

typedef struct pt_regs {
    uint64_t rdi;  // First argument
    uint64_t rsi;  // Second argument
    uint64_t rdx;  // Third argument
    uint64_t rcx;
    uint64_t rax;  // Syscall number
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t rbx;
    // uint64_t rsp;
    // uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // uint64_t rip;
    // uint64_t cs;
    // uint64_t eflags;
    // uint64_t rsp;
    // uint64_t ss;
} pt_regs;

enum SYSCALL_NR {
    sys_printf,
    sys_getchar,
    sys_time,
    sys_date,
    sys_sleep,
    sys_abort,
    sys_malloc,
    sys_free,
    sys_print_heap,
    sys_stdin_clear,
    sys_shutdown,
    sys_tty_init,
    sys_ursp
};

#if defined(__is_libk)
// Syscall Table
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

// #define __NR_syscalls 256
typedef int64_t (*syscall_t)();
static const syscall_t SYSCALL_TABLE[] = {
    [sys_printf] = printf,
    [sys_getchar] = getchar,
    [sys_time] = time,
    [sys_date] = date,
    [sys_sleep] = sleep,
    [sys_abort] = abort,
    [sys_malloc] = malloc,
    [sys_free] = free,
    [sys_print_heap] = print_heap,
    [sys_stdin_clear] = stdin_clear,
    [sys_shutdown] = shutdown,
    [sys_tty_init] = tty_init,
    [sys_ursp] = ursp
};

#pragma GCC diagnostic pop
// #endif

static inline uint64_t read_msr(uint32_t msr) {
    uint64_t value;
    __asm__ volatile ("rdmsr" : "=A"(value) : "c"(msr));
    return value;
}

static inline void write_msr(uint32_t msr, uint32_t lo, uint32_t hi) {
    __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void enable_syscall();
extern void syscall_entry();
void configure_segments();

int64_t syscall_handler(pt_regs *regs);
#endif
void init_syscall();

#define SYSCALL_REGS_MAX 6
#define SYSCALL_ARGS_MAX 12

/**
 * @brief A format function of `vsyscall()`. See x86_64 calling conventions. (Same as `vsyscall()`,
 * but with the first argument outside of va_list)
 * 
 * @param number 
 * @param rdi 
 * @param args 
 * @return uint64_t 
 */
static uint64_t fvsyscall(long number, volatile uint64_t rdi, va_list args) {
    volatile uint64_t ret;

    // Extract the arguments and load them into the correct registers
    volatile uint64_t rsi = va_arg(args, uint64_t);
    volatile uint64_t rdx = va_arg(args, uint64_t);
    volatile uint64_t r10 = va_arg(args, uint64_t);
    volatile uint64_t r8 = va_arg(args, uint64_t);
    volatile uint64_t r9 = va_arg(args, uint64_t);

    // Perform the syscall
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(number), "D"(rdi), "S"(rsi), "d"(rdx), "r"(r8), "r"(r9), "r"(r10) // rdi, rsi, rdx, r8, r9, r10
        : "memory", "cc"
    );

    return ret;
}

/**
 * @brief A variadic function of `syscall()`. See x86_64 calling conventions.
 * 
 * @param number 
 * @param args 
 * @return uint64_t 
 */
static uint64_t vsyscall(long number, va_list args) {
    volatile uint64_t rdi = va_arg(args, uint64_t);

    return fvsyscall(number, rdi, args);
}

/**
 * @brief Executes a system call with the specified number and arguments.
 *
 * This function performs a syscall instruction. The arguments are processed using
 * variadic arguments (va_list). The syscall uses x86_64 calling conventions
 * where the syscall number is placed in the RAX register, and the first three
 * arguments are placed in the RDI, RSI, and RDX registers respectively.
 * 
 * @Note: For now is limited to 6 arguments (rdi, rsi, rdx, r10, r8, r9)
 *
 * @param number The syscall number to execute.
 * @param ... A variable number of arguments to pass to the syscall.
 * @return uint64_t The return value of the syscall.
 */
static inline uint64_t syscall(long number, ...) {
    va_list args;
    va_start(args, number);

    volatile uint64_t ret = vsyscall(number, args);

    va_end(args);

    return ret;
}

#endif
