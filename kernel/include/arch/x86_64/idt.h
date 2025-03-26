//  idt.h: Manages the Interrupt Descriptor Table (IDT)

#ifndef IDT_H

#define IDT_H

#include <types.h>

#define KERNEL_CS_SELECTOR_OFFSET_GDT 0x08
#define KERNEL_SS_SELECTOR_OFFSET_GDT 0x10
#define KERNEL_DS_SELECTOR_OFFSET_GDT 0x10
#define KERNEL_ES_SELECTOR_OFFSET_GDT 0x10
#define KERNEL_GS_SELECTOR_OFFSET_GDT 0x10
#define KERNEL_ES_SELECTOR_OFFSET_GDT 0x10
#define IDT_ENTRIES_AMOUNT 256
#define INT_TYPE_ATTRIBUTES 0b10001110

typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint64_t base;
} idt_register;

typedef struct __attribute__((packed)) {
  uint16_t offset_low;      // offset bits 0..15
  uint16_t selector;        // a code segment selector in GDT or LDT
  uint8_t ist;              // bits 0..2 holds Interrupt Stack Table offset, The rest of the bits are zero.
  uint8_t type_attributes;  // Attributes bits
                                //      - Bit 7:     Present bit - should be 1
                                //      - Bits 6-5:  Privilege level of caller (0=kernel..3=user)
                                //      - Bit 4:     Should always be 0
                                //      - Bits 3-0:  Gate type - 0b1110 = Interrupt Gate, 0b1111 = Trap Gate 
  uint16_t offset_medium;   // offset bits 16..31
  uint32_t offset_high;     // offset bits 32..63
  uint32_t zero;            // reserved
} idt_gate_descriptor;

/**
* Updates the idt by setting its address
*/
void update_idt();


/**
* @brief Adds an interrupt handler for a specified interrupt number 

* The interrupt handler must have __attribute__((interrupt)) and can receive struct interrupt_frame* as an argument
*/
void idt_gate_init(uint8_t gate_descriptor_num, void (*handler)());

#endif // IDT_H
