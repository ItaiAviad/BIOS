// Syscalls

#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <proc.h>
#include <file_op.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <kernel.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_KERNEL_GS_BASE 0xC0000102

#define EFER_SCE (1 << 0)  // SCE bit is the lowest bit in MSR_EFER
#define EFER_LME (1ULL << 8)  // Long Mode Enable


enum SYSCALL_NR {
    sys_exit = 0,
    sys_exec,
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
    sys_stdin_insert,
    sys_shutdown,
    sys_tty_init,
    sys_scanf,
    sys_memset,
    sys_memcpy,
    sys_fgets,
    sys_open,
    sys_close,
    sys_read,
    sys_write,
    sys_lseek,
    sys_list_dir,
    sys_path_exists,
    sys_send_dns_request,
};

#if defined(__is_libk)
// Syscall Table
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

// #define __NR_syscalls 256
typedef int64_t (*syscall_t)();
static const syscall_t SYSCALL_TABLE[] = {
    [sys_exit] = exit, // Todo add sys_exit,
    [sys_exec] = exec,
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
    [sys_stdin_insert] = stdin_insert,
    [sys_shutdown] = shutdown,
    [sys_tty_init] = tty_init,
    [sys_scanf] = scanf,
    [sys_memset] = memset,
    [sys_memcpy] = memcpy,
    [sys_fgets] = fgets,
    [sys_open] = open,
    [sys_close] = close,
    [sys_write] = write,
    [sys_read] = read,
    [sys_lseek] = lseek,
    [sys_list_dir] = list_dir,
    [sys_path_exists] = path_exists,
    [sys_send_dns_request] = send_dns_request,
};

#pragma GCC diagnostic pop
// #endif

static inline uint64_t read_msr(uint32_t msr) {
    uint64_t value;
    __asm__ volatile ("rdmsr" : "=A"(value) : "c"(msr));
    return value;
}

static inline void write_msr(uint32_t msr, uint64_t value) {
    asm volatile (
        "wrmsr" 
        : 
        : "c"(msr), "a"((uint32_t)value), "d"((uint32_t)(value >> 32))
    );
}

void enable_syscall();
extern void syscall_entry();
void configure_segments(uint16_t kernel_cs, uint16_t kernel_ss, uint16_t user_cs, uint16_t user_ss);

int64_t syscall_handler(cpu_state *regs);

struct kernel_gs_base {
    uint64_t kstack;
    uint64_t ustack;
};
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
        "sub rsp, 24\n\t"   // Reserve 24 bytes for eflags, rip, rsp
        "push rdi\n\t"
        "push rsi\n\t"
        "push rdx\n\t"
        "push rcx\n\t"
        "push rax\n\t"
        "push r8\n\t"
        "push r9\n\t"
        "push r10\n\t"
        "push r11\n\t"
        "push rbx\n\t"
        "push rbp\n\t"
        "push r12\n\t"
        "push r13\n\t"
        "push r14\n\t"
        "push r15\n\t"
        "mov rbx, rsp \n\t"
        "syscall\n\t"
        "mov rsp, rbx \n\t"
        "pop r15\n\t"
        "pop r14\n\t"
        "pop r13\n\t"
        "pop r12\n\t"
        "pop rbp\n\t"
        "pop rbx\n\t"
        "pop r11\n\t"
        "pop r10\n\t"
        "pop r9\n\t"
        "pop r8\n\t"
        "pop rcx\n\t" // Can't restore rax beacuse of return val from syscall
        "pop rcx\n\t"
        "pop rdx\n\t"
        "pop rsi\n\t"
        "pop rdi\n\t"
        "add rsp, 24\n\t"
        : "=a"(ret)                          // Output: return value in rax
        : "a"(number), "D"(rdi), "S"(rsi), "d"(rdx), "r"(r8), "r"(r9), "r"(r10) // Inputs
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
