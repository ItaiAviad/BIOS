#include <arch/x86_64/gdt.h>
#include <arch/x86_64/tss.h>

gdt_entry gdt[3]; // one null segment, two ring 0 segments, two ring 3 segments,
                  // TSS segment
gdt_register gdt_reg;

void init_gdt() {
    // (ring 0 segments)
    gdt_reg.limit = sizeof(gdt_entry) * 3  - 1;
    gdt_reg.base = (uint64_t)gdt;

    // Null Segment (0x0)
    gdt[0].limit = 0x0;
    gdt[0].base_low = 0x0;
    gdt[0].base_middle = 0x0;
    gdt[0].access = 0x0;
    gdt[0].granularity = 0x0;
    gdt[0].base_high = 0x0;
    gdt[0].base32 = 0x0;
    gdt[0].reserved = 0x0;

    // Kernel Code Segment (0x10)
    gdt[1].limit = 0xFFFF;
    gdt[1].base_low = 0x0;
    gdt[1].base_middle = 0x0;
    gdt[1].access = 0b10011010;            // 0b10011010
    gdt[1].granularity = 0b10101111; // 0b11001111
    gdt[1].base_high = 0x0;
    gdt[1].base32 = 0x0;
    gdt[1].reserved = 0x0;

    // Kernel Data Segment (0x20)
    gdt[2].limit = 0xFFFF;
    gdt[2].base_low = 0x0;
    gdt[2].base_middle = 0x0;
    gdt[2].access = 0b10010010;            // 0b10010010
    gdt[2].granularity = 0b10100000; // 0b11001111
    gdt[2].base_high = 0x0;
    gdt[2].base32 = 0x0;
    gdt[2].reserved = 0x0;
    // Ring 3 code segment(0x30)
    gdt_entry_bits *ring3_code = (gdt_entry_bits *)&gdt[3];
    // Ring 3 data segment(0x40)
    gdt_entry_bits *ring3_data = (gdt_entry_bits *)&gdt[4];

    ring3_code->limit_low = 0xFFFF;
    ring3_code->base_low = 0;
    ring3_code->accessed = 0;
    ring3_code->read_write = 1; // since this is a code segment, specifies that
                                // the segment is readable
    ring3_code->conforming_expand_down = 0;
    // does not matter for ring 3 as no lower privilege level exists
    ring3_code->code = 1;
    ring3_code->code_data_segment = 1;
    ring3_code->DPL = 3; // ring 3
    ring3_code->present = 1;
    ring3_code->limit_high = 0xF;
    ring3_code->available = 1;
    ring3_code->long_mode = 1; // Long mode
    ring3_code->big = 1;       // 32 bits
    ring3_code->gran = 1;      // 4KB page addressing
    ring3_code->base_high = 0;
    ring3_code->base32 = 0x0;
    ring3_code->reserved = 0x0;

    *ring3_data = *ring3_code; // contents are similar so save time by copying
    ring3_data->code = 0;      // not code but data
    // Tss(0x50)
    init_tss((gdt_entry_bits*)(&gdt[5])); // TSS segment will be the fifth

    load_gdt(gdt_reg.limit, gdt_reg.base);

    // flush_tss();
}

void test_user_function(void) {}
