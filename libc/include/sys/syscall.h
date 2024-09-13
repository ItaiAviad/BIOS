// Syscalls

#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
#include <stdint.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082

#define EFER_SCE (1 << 0)  // SCE bit is the lowest bit in MSR_EFER

struct pt_regs {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rax;  // Syscall number
    uint64_t rcx;
    uint64_t rdx;  // Third argument
    uint64_t rsi;  // Second argument
    uint64_t rdi;  // First argument
    // uint64_t rip;
    // uint64_t cs;
    // uint64_t eflags;
    // uint64_t rsp;
    // uint64_t ss;
};


#if defined(__is_libk)
static inline uint64_t read_msr(uint32_t msr) {
    uint64_t value;
    __asm__ volatile ("rdmsr" : "=A"(value) : "c"(msr));
    return value;
}

static inline void write_msr(uint32_t msr, uint64_t value) {
    __asm__ volatile ("wrmsr" :: "c"(msr), "A"(value));
}

void enable_syscall();
extern void syscall_entry();
void configure_segments();

void syscall_handler(struct pt_regs *regs);
#endif
void init_syscall();

static inline uint64_t syscall(uint64_t syscall_number, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    uint64_t ret;
    __asm__ volatile (
        "movq %1, %%rax;"  // Syscall number
        "movq %2, %%rdi;"  // First argument
        "movq %3, %%rsi;"  // Second argument
        "movq %4, %%rdx;"  // Third argument
        "syscall;"         // Make the syscall
        "movq %%rax, %0;"  // Return value
        : "=r"(ret)        // Output
        : "r"(syscall_number), "r"(arg0), "r"(arg1), "r"(arg2)  // Inputs
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory"    // Clobbered registers
    );
    return ret;
}

#endif
