// isr.h: Handle ISRs (Interrupt Service Routines)

#ifndef ISR_H
#define ISR_H

#include <types.h>

/** 
  Structs that define the argument for an isr handler
  
  Sources: https://os.phil-opp.com/returning-from-exceptions/exception-stack-frame.svg,
           https://docs.rs/x86_64/0.1.1/x86_64/structures/idt/struct.ExceptionStackFrame.html
*/
typedef struct __attribute__((packed)) {
    uint64_t instruction_pointer;
    uint64_t code_segment;
    uint64_t cpu_flags;
    uint64_t stack_pointer;
    uint64_t stack_segment;
    uint64_t error_code;
} isr_frame;

typedef struct __attribute__((packed)) {
    uint64_t instruction_pointer;
    uint64_t code_segment;
    uint64_t cpu_flags;
    uint64_t stack_pointer;
    uint64_t stack_segment;
} isr_frame_without_err_code;



/**
* Function to handle ISRs called by the cpu
*/
void isr_handler(uint64_t isr_num, isr_frame* isr_args);

/**
* Declares, Sets the isr handlers in the idt
*/
void install_isr_handlers();


#define handle_isr_without_err_code(isr_num)\
    void __attribute__((interrupt)) handle_##isr_num##_isr(isr_frame_without_err_code* isr_args) {\
        isr_frame isr_args_full = {\
            .instruction_pointer = isr_args->instruction_pointer,\
            .code_segment =  isr_args->code_segment,\
            .cpu_flags = isr_args->cpu_flags,\
            .stack_pointer = isr_args->stack_pointer,\
            .stack_segment = isr_args->stack_segment,\
            .error_code = 0\
        };\
        isr_handler(isr_num, &isr_args_full);\
    }

#define handle_isr_err_code(isr_num)\
    void __attribute__((interrupt)) handle_##isr_num##_isr(isr_frame* isr_args) {\
        isr_handler(isr_num, isr_args);\
    }

#define NUM_OF_ISR_INT 32

#endif // ISR_H
