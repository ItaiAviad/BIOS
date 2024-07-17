// isr.h: Handle ISRs (Interrupt Service Routines)

#ifndef ISR_H
#define ISR_H

#define NUM_OF_ISR_INT 32

#include <types.h>




// Structure for pushed registers saved for ISR
typedef struct __attribute__((packed)) {
    // Define the callee-saved registers
    uint64_t r15, r14, r13, r12, rbp, rbx;

    // Define the callee-clobbered registers
    uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;

    // Define the IRQ Number and the error code
    uint64_t irq_number, error_code;

    // Define the return frame for the iretq call
    uint64_t rip, cs, eflags, rsp, ss;
} registers;

/**
 * Function to handle ISRs called by the cpu
 */
void isr_handler(uint64_t isr_num,uint64_t error_code, registers* regs);

/**
 * Declares, Sets the isr handlers in the idt
 */
void install_isr_handlers();


extern void handle_0_isr();
extern void handle_1_isr();
extern void handle_2_isr();
extern void handle_3_isr();
extern void handle_4_isr();
extern void handle_5_isr();
extern void handle_6_isr();
extern void handle_7_isr();
extern void handle_8_isr();
extern void handle_9_isr();
extern void handle_10_isr();
extern void handle_11_isr();
extern void handle_12_isr();
extern void handle_13_isr();
extern void handle_14_isr();
extern void handle_15_isr();
extern void handle_16_isr();
extern void handle_17_isr();
extern void handle_18_isr();
extern void handle_19_isr();
extern void handle_20_isr();
extern void handle_21_isr();
extern void handle_22_isr();
extern void handle_23_isr();
extern void handle_24_isr();
extern void handle_25_isr();
extern void handle_26_isr();
extern void handle_27_isr();
extern void handle_28_isr();
extern void handle_29_isr();
extern void handle_30_isr();
extern void handle_31_isr();


#endif // ISR_H
