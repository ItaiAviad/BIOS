[bits 64]


[extern isr_handler]

REGISTER_SIZE:          equ 0x78
QUADWORD_SIZE:          equ 0x08

%macro PUSHALL 0
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
%endmacro


%macro POPALL 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
%endmacro


%macro SAVE_REGS_AND_CALL_HANDLER 1
    ; Save all registers since calling context is unknown
    PUSHALL

    ; Configure arguments for the method call (Using SYSV ABI)
    ; 1-st parm - RDI Should contain the interrupt Number
    ; 2-st parm - RSI Should contain the error code
    ; 3-rd parm - RDX Should contain the pointer to the registers (AKA the stack pointer)
    ; Source: https://wiki.osdev.org/System_V_ABI
    mov rdi, [rsp + REGISTER_SIZE]                      ; ISR Number is last on the stack
    mov rsi, [rsp + REGISTER_SIZE + QUADWORD_SIZE]      ; Error Code is first on the stack
    mov rdx, rsp

    call %1

    ; Restore all registers before returning
    POPALL
%endmacro

%macro ISR_NOERRCODE 1
  global handle_%1_isr
  handle_%1_isr:
    cli

    push qword 0 ; Push a zero error code into the stack
    push qword %1

    SAVE_REGS_AND_CALL_HANDLER isr_handler

    ; Pop the stack so iretq would find the IP
    add rsp, 0x10

    sti

    iretq
%endmacro

%macro ISR_ERRCODE 1
  global handle_%1_isr
  handle_%1_isr:
    cli

    push qword %1

    SAVE_REGS_AND_CALL_HANDLER isr_handler

    ; Pop the stack so iretq would find the IP
    add rsp, 0x10

    sti

    iretq
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
; The 8-th ISR has cases when it has an error code and needs special handling 
ISR_NOERRCODE 8

ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
