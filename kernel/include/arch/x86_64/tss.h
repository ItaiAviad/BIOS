// TSS definitions (for Userspace)

#ifndef TSS_H

#define TSS_H

#include <types.h>
#include <string.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/gdt.h>

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
// See: https://wiki.osdev.org/Task_State_Segment#Long_Mode
typedef struct __attribute__((packed)) {
	uint32_t reserved6;     // The stack pointer to load when changing to kernel mode.
	uint32_t rsp0_low;      // The stack segment to load when changing to kernel mode.
	uint32_t rsp0_high;      // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	uint32_t rsp1_low;
	uint32_t rsp1_high;
	uint32_t rsp2_low;
	uint32_t rsp2_high;
	uint32_t reserved5;
	uint32_t reserved4;
	uint32_t ist1_low;
	uint32_t ist1_high;
	uint32_t ist2_low;
	uint32_t ist2_high;
	uint32_t ist3_low;
	uint32_t ist3_high;
	uint32_t ist4_low;
	uint32_t ist4_high;
	uint32_t ist5_low;
	uint32_t ist5_high;
	uint32_t ist6_low;
	uint32_t ist6_high;
	uint32_t ist7_low;
	uint32_t ist7_high;
	uint32_t reserved3;
	uint32_t reserved2;
	uint16_t reserved1;
	uint16_t iomap_base;
} tss_entry_t;

extern void flush_tss();
// extern void jump_usermode();

void init_tss(gdt_entry_bits *g);
void set_kernel_stack(uint32_t stack);

#endif
