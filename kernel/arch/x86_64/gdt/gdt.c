#include <arch/x86_64/gdt.h>

gdt_register gdt_reg;

void init_userspace() {
    cli();

    static gdt_entry_bits gdt[6]; // one null segment, two ring 0 segments, two ring 3 segments, TSS segment
    // (ring 0 segments)

    gdt_entry_bits *ring3_code = &gdt[3];
    gdt_entry_bits *ring3_data = &gdt[4];

    ring3_code->limit_low = 0xFFFF;
    ring3_code->base_low = 0;
    ring3_code->accessed = 0;
    ring3_code->read_write = 1; // since this is a code segment, specifies that the segment is readable
    ring3_code->conforming_expand_down = 0; // does not matter for ring 3 as no lower privilege level exists
    ring3_code->code = 1;
    ring3_code->code_data_segment = 1;
    ring3_code->DPL = 3; // ring 3
    ring3_code->present = 1;
    ring3_code->limit_high = 0xF;
    ring3_code->available = 1;
    ring3_code->long_mode = 0;
    ring3_code->big = 1; // it's 32 bits
    ring3_code->gran = 1; // 4KB page addressing
    ring3_code->base_high = 0;

    *ring3_data = *ring3_code; // contents are similar so save time by copying
    ring3_data->code = 0; // not code but data

    write_tss(&gdt[5]); // TSS segment will be the fifth 
    // set_kernel_stack(stack);

    gdt_reg.base = (uint64_t)&gdt;
    gdt_reg.limit = (6 * sizeof(gdt_entry_bits)) - 1;
    __asm__ volatile("lgdt (%0)" :: "m"(gdt_reg));

    flush_tss();

    jump_usermode();
}

void test_user_function(void) {
    
}
