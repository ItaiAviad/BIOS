// Syscalls

#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
#include <stdarg.h>
#include <stdio.h>

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
    uint64_t rsp;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    // Additional arguments
    // uint64_t arg11;
    // uint64_t arg10;
    // uint64_t arg9;
    // uint64_t arg8;
    // uint64_t arg7;
    // uint64_t arg6;

    // uint64_t r15;
    // uint64_t r14;
    // uint64_t r13;
    // uint64_t r12;
    // uint64_t rbp;
    // uint64_t rsp;
    // uint64_t rbx;
    // uint64_t r11;
    // uint64_t r10;
    // uint64_t r9;
    // uint64_t r8;
    // uint64_t rax;  // Syscall number
    // uint64_t rcx;
    // uint64_t rdx;  // Third argument
    // uint64_t rsi;  // Second argument
    // uint64_t rdi;  // First argument

    // uint64_t rip;
    // uint64_t cs;
    // uint64_t eflags;
    // uint64_t rsp;
    // uint64_t ss;
} pt_regs;

enum SYSCALL_NR {
    sys_printf
};

#if defined(__is_libk)
// Syscall Table
#define __NR_syscalls 256
// typedef uint64_t (*syscall_t)();
typedef long syscall_t();
typedef void (*funcptr)();
static const funcptr SYSCALL_TABLE[] = {
    [sys_printf] = printf
};
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

void syscall_handler(pt_regs *regs);
#endif
void init_syscall();

#define SYSCALL_REGS_MAX 6
#define SYSCALL_ARGS_MAX 12

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
static inline uint64_t syscall(volatile long number, ...) {
    volatile uint64_t ret;

    va_list args;
    va_start(args, number);
    
    // push args
    // volatile uint64_t arg;

    volatile uint64_t rdi = va_arg(args, uint64_t);
    volatile uint64_t rsi = va_arg(args, uint64_t);
    volatile uint64_t rdx = va_arg(args, uint64_t);
    volatile uint64_t r10 = va_arg(args, uint64_t);
    volatile uint64_t r8 = va_arg(args, uint64_t);
    volatile uint64_t r9 = va_arg(args, uint64_t);
    
    // Push additional arguments (arg6, ...)
    // for (int i = 0; i < SYSCALL_ARGS_MAX - SYSCALL_REGS_MAX; ++i) {
    //     arg = va_arg(args, uint64_t);
    //     __asm__ volatile("push %0;" : : "r"(arg));
    // }

    // Push r10, r8, r9
    __asm__ volatile("push %0;" : : "r"(r10));
    __asm__ volatile("push %0;" : : "r"(r8));
    __asm__ volatile("push %0;" : : "r"(r9));

    // rax, rdi, rsi, rdx, syscall
    __asm__ volatile("syscall" : "=a"(ret) : "a"(number), "D"(rdi), "S"(rsi), "d"(rdx) : "memory","cc");

    // Pop additional arguments (arg6, ...)
    // for (int i = 0; i < SYSCALL_ARGS_MAX - SYSCALL_REGS_MAX; ++i) {
    //     __asm__ volatile("pop %0;" : "=r"(arg));
    // }

    va_end(args);

    return ret;
}

#endif
