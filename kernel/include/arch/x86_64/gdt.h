// GDT definitions (for Userspace)

#ifndef GDT_H

#define GDT_H

#include <types.h>
#include <string.h>
#include <arch/x86_64/interrupts.h>

#define GDT_ENTRIES 6

typedef struct __attribute__((packed)) {
    uint16_t limit;         // 20 bit maximum addressable unit
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;         // access to segment
    uint8_t granularity;    // contains limit and flags
    uint8_t base_high;      // 32 base contains base linear address of segment
} gdt_entry;

typedef struct __attribute__((packed)) {
	unsigned int limit_low              : 16;
	unsigned int base_low               : 24;
	unsigned int accessed               :  1;
	unsigned int read_write             :  1; // readable for code, writable for data
	unsigned int conforming_expand_down :  1; // conforming for code, expand down for data
	unsigned int code                   :  1; // 1 for code, 0 for data
	unsigned int code_data_segment      :  1; // should be 1 for everything but TSS and LDT
	unsigned int DPL                    :  2; // privilege level
	unsigned int present                :  1;
	unsigned int limit_high             :  4;
	unsigned int available              :  1; // only used in software; has no effect on hardware
	unsigned int long_mode              :  1;
	unsigned int big                    :  1; // 32-bit opcodes for code, uint32_t stack for data
	unsigned int gran                   :  1; // 1 to use 4k page addressing, 0 for byte addressing
	unsigned int base_high              :  8;
} gdt_entry_bits;

typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint64_t base;
} gdt_register;

extern void load_gdt();
extern void reloadSegments();

void init_gdt();
// void init_userspace();
void test_user_function(void);

#endif
