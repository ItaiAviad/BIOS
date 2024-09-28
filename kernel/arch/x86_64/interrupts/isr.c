#include <arch/x86_64/isr.h>
#include <stdint.h>

// An array of strings in which exception_messages[i] specifies the i-th interrupt error code
char *isr_exception_messages[] = {
    "Division by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void init_isr_handlers() {
    // Exception handlers
    idt_gate_init(0, handle_0_isr);
    idt_gate_init(1, handle_1_isr);
    idt_gate_init(2, handle_2_isr);
    idt_gate_init(3, handle_3_isr);
    idt_gate_init(4, handle_4_isr);
    idt_gate_init(5, handle_5_isr);
    idt_gate_init(6, handle_6_isr);
    idt_gate_init(7, handle_7_isr);
    idt_gate_init(8, handle_8_isr);
    idt_gate_init(9, handle_9_isr);
    idt_gate_init(10, handle_10_isr);
    idt_gate_init(11, handle_11_isr);
    idt_gate_init(12, handle_12_isr);
    idt_gate_init(13, handle_13_isr);
    idt_gate_init(14, handle_14_isr);
    idt_gate_init(15, handle_15_isr);
    idt_gate_init(16, handle_16_isr);
    idt_gate_init(17, handle_17_isr);
    idt_gate_init(18, handle_18_isr);
    idt_gate_init(19, handle_19_isr);
    idt_gate_init(20, handle_20_isr);
    idt_gate_init(21, handle_21_isr);
    idt_gate_init(22, handle_22_isr);
    idt_gate_init(23, handle_23_isr);
    idt_gate_init(24, handle_24_isr);
    idt_gate_init(25, handle_25_isr);
    idt_gate_init(26, handle_26_isr);
    idt_gate_init(27, handle_27_isr);
    idt_gate_init(28, handle_28_isr);
    idt_gate_init(29, handle_29_isr);
    idt_gate_init(30, handle_30_isr);
    idt_gate_init(31, handle_31_isr);
    // IRQs (PIC1 - 0x20-0x27, PIC2 - 0x28-0x2F)
    idt_gate_init(32, handle_32_isr);
    idt_gate_init(33, handle_33_isr);
    idt_gate_init(34, handle_34_isr);

    update_idt();
}

void isr_handler(uint64_t isr_num, uint64_t error_code, registers* regs){
    cli();// Disable interrupts to prevent getting the same interrupt regenerated while handling one 

    if (isr_num <= 31) {
        printf("ISR: %s (%d) called, rip: %d, cr2: %d, error_code: %d \n", isr_exception_messages[isr_num], isr_num, regs->rip, regs->cr2, (uint64_t)error_code);
        (regs->rip)++;
        if(isr_num == 0){
            return;
        }
        while(true){
            
        }
    }
    else if (isr_num >= 32) {
        if (isr_num == IRQ_PIT + PIC1_OFFSET) // PIT IRQ
            pit_handler();
        else if (isr_num == IRQ_KEYBOARD + PIC1_OFFSET) // Keyboard IRQ
            buffer_put(inb(PS2_KEYBOARD_PORT_DATA));

        pic_send_eoi(isr_num - PIC1_OFFSET); // Send ACK to PIC

        if(isr_num == 34){
            printf("Isr: %d", isr_num);
        }
    }

    sti(); //ReEnable interrupts
}


