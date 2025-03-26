#include <arch/x86_64/tss.h>

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdtr_t;

// Function to get TSS address from GDT
uint64_t get_tss_addr(void) {
    gdtr_t gdtr;
    uint16_t tr_selector;
    uint64_t tss_desc_addr;
    uint64_t tss_addr;

    // Step 1: Get GDT base from GDTR
    asm volatile ("sgdt %0" : "=m" (gdtr) : : "memory");

    // Step 2: Get TSS selector from TR
    asm volatile ("str %0" : "=r" (tr_selector) : : );

    // Step 3: Calculate TSS descriptor address
    // Mask off RPL/TI bits (selector index * 8)
    tss_desc_addr = gdtr.base + (tr_selector & 0xFFF8);

    // Step 4: Extract TSS base address from descriptor
    // TSS descriptor is 16 bytes, base is scattered across offsets
    uint8_t* desc = (uint8_t*)tss_desc_addr;
    tss_addr = (uint64_t)(desc[2] | (desc[3] << 8) |     // Base 0-15
                         (desc[4] << 16) |               // Base 16-23
                         (desc[7] << 24) |               // Base 24-31
                         ((uint64_t)*(uint32_t*)&desc[8] << 32)); // Base 32-63

    return tss_addr;
}