// TSS definitions (for Userspace)

#ifndef TSS_H

#define TSS_H

#include <types.h>
#include <string.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/gdt.h>

#define TSS_SIZE 104

// TSS 32bit (Protected Mode):
// typedef struct __attribute__((packed)) {
// 	uint32_t prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
// 	uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
// 	uint32_t ss0;      // The stack segment to load when changing to kernel mode.
// 	// Everything below here is unused.
// 	uint32_t esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
// 	uint32_t ss1;
// 	uint32_t esp2;
// 	uint32_t ss2;
// 	uint32_t cr3;
// 	uint32_t eip;
// 	uint32_t eflags;
// 	uint32_t eax;
// 	uint32_t ecx;
// 	uint32_t edx;
// 	uint32_t ebx;
// 	uint32_t esp;
// 	uint32_t ebp;
// 	uint32_t esi;
// 	uint32_t edi;
// 	uint32_t es;
// 	uint32_t cs;
// 	uint32_t ss;
// 	uint32_t ds;
// 	uint32_t fs;
// 	uint32_t gs;
// 	uint32_t ldt;
// 	uint16_t trap;
// 	uint16_t iomap_base;
// } tss_entry_t;

// TSS 64bit (Long Mode):
// See: https://wiki.osdev.org/Task_State_Segment#Long_Mode
typedef struct __attribute__((packed)) {
	uint32_t reserved0;
    uint64_t rsp0;       // Stack pointer for ring 0
    uint64_t rsp1;       // Stack pointer for ring 1
    uint64_t rsp2;       // Stack pointer for ring 2
    uint64_t reserved1;
    uint64_t ist1;       // Interrupt Stack Table 1
    uint64_t ist2;       // Interrupt Stack Table 2
    uint64_t ist3;       // Interrupt Stack Table 3
    uint64_t ist4;       // Interrupt Stack Table 4
    uint64_t ist5;       // Interrupt Stack Table 5
    uint64_t ist6;       // Interrupt Stack Table 6
    uint64_t ist7;       // Interrupt Stack Table 7
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;       // I/O Map Base Address
} tss_entry_t;

extern void flush_tss();
// extern void jump_usermode();

void init_tss(gdt_entry_bits *g);
void set_kernel_stack(uint32_t stack);
uint64_t get_tss_addr(void);

#endif
