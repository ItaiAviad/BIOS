#include <cpu/idt.h>

idt_gate_descriptor idt[IDT_ENTRIES_AMOUNT];
idt_register idt_reg;

void set_idt() {
  idt_reg.base = (uint64_t)&idt;
  idt_reg.limit = (IDT_ENTRIES_AMOUNT * sizeof(idt_gate_descriptor)) - 1;
  __asm__ volatile("lidt (%0)" ::"r"(&idt_reg));
}

void add_int_idt(uint8_t gate_descriptor_num, uint64_t handler_addr){
  idt_gate_descriptor idt_gate = {
    .offset_low = handler_addr & 0xFFFF,
    .offset_meduim =  (handler_addr >> 0x10) & 0xFFFF,
    .offset_high = (handler_addr >> 0x20) & 0xFFFFFFFF,
    .selector = KERNEL_CS_SELECTOR_OFFSET_GDT,
    .ist = 0x0,
    .type_attributes = INT_TYPE_ATTRIBUTES,
    .zero = 0x0
  };

  idt[gate_descriptor_num] = idt_gate;
}