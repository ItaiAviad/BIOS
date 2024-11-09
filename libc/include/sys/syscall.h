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
    sys_getchar,
    sys_printf
};

// #if defined(__is_libk)
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

void syscall_handler(struct pt_regs *regs);
void init_syscall();
#endif

#define SYSCALL_ARGS_MAX 6

static inline uint64_t syscall(long number, ...) {
    uint64_t ret;

    va_list args;
    va_start(args, number);
    
    // push args
    __asm__ volatile("push %0;" : : "r"(number));
    for (int i = 0; i < SYSCALL_ARGS_MAX; i++) {
        uint64_t arg = va_arg(args, uint64_t);
        __asm__ volatile("push %0;" : : "r"(arg));
    }

    __asm__ volatile (
        "syscall;"          // Make the syscall
        : "=a"(ret)         // Output: rax holds the return value
    );

    va_end(args);

    return ret;
}

// #endif__asm__
