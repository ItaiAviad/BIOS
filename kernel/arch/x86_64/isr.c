#include "arch/x86_64/idt.h"
#include <arch/x86_64/isr.h>
#include <stdio.h>

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


handle_isr_without_err_code(0)
handle_isr_without_err_code(1)
handle_isr_without_err_code(2)
handle_isr_without_err_code(3)
handle_isr_without_err_code(4)
handle_isr_without_err_code(5)
handle_isr_without_err_code(6)
handle_isr_without_err_code(7)
handle_isr_without_err_code(8)
handle_isr_without_err_code(9)
handle_isr_err_code(10)
handle_isr_err_code(11)
handle_isr_err_code(12)
handle_isr_err_code(13)
handle_isr_err_code(14)
handle_isr_without_err_code(15)
handle_isr_without_err_code(16)
handle_isr_without_err_code(17)
handle_isr_without_err_code(18)
handle_isr_without_err_code(19)
handle_isr_without_err_code(20)
handle_isr_without_err_code(21)
handle_isr_without_err_code(22)
handle_isr_without_err_code(23)
handle_isr_without_err_code(24)
handle_isr_without_err_code(25)
handle_isr_without_err_code(26)
handle_isr_without_err_code(27)
handle_isr_without_err_code(28)
handle_isr_without_err_code(29)
handle_isr_without_err_code(30)
handle_isr_without_err_code(31)

void install_isr_handlers(){
    set_int_handler_idt(0, handle_0_isr);
    set_int_handler_idt(1, handle_1_isr);
    set_int_handler_idt(2, handle_2_isr);
    set_int_handler_idt(3, handle_3_isr);
    set_int_handler_idt(4, handle_4_isr);
    set_int_handler_idt(5, handle_5_isr);
    set_int_handler_idt(6, handle_6_isr);
    set_int_handler_idt(7, handle_7_isr);
    set_int_handler_idt(8, handle_8_isr);
    set_int_handler_idt(9, handle_9_isr);
    set_int_handler_idt(10, handle_10_isr);
    set_int_handler_idt(11, handle_11_isr);
    set_int_handler_idt(12, handle_12_isr);
    set_int_handler_idt(13, handle_13_isr);
    set_int_handler_idt(14, handle_14_isr);
    set_int_handler_idt(15, handle_15_isr);
    set_int_handler_idt(16, handle_16_isr);
    set_int_handler_idt(17, handle_17_isr);
    set_int_handler_idt(18, handle_18_isr);
    set_int_handler_idt(19, handle_19_isr);
    set_int_handler_idt(20, handle_20_isr);
    set_int_handler_idt(21, handle_21_isr);
    set_int_handler_idt(22, handle_22_isr);
    set_int_handler_idt(23, handle_23_isr);
    set_int_handler_idt(24, handle_24_isr);
    set_int_handler_idt(25, handle_25_isr);
    set_int_handler_idt(26, handle_26_isr);
    set_int_handler_idt(27, handle_27_isr);
    set_int_handler_idt(28, handle_28_isr);
    set_int_handler_idt(29, handle_29_isr);
    set_int_handler_idt(30, handle_30_isr);
    set_int_handler_idt(31, handle_31_isr);
    update_idt();
}

void isr_handler(uint64_t isr_num, isr_frame* isr_args){
    printf("isr: %s(%d) called", isr_exception_messages[isr_num], isr_num);
}


