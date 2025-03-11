section .text
global jump_usermode

; Function to jump to user mode
; Parameters:
;   - rdi: Start address of the user mode code (binary entry point)
;   - rsi: Stack pointer for user mode (initial stack)
jump_usermode:
    ; Set up segment registers for user mode (Ring 3)
    mov ax, 0x20 | 3         ; Ring 3 data selector with RPL 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax               ; SS is handled by iretq

    ; Push SS selector + RPL = 3 (Ring 3)
    push 0x20 | 3
    ; Push RSP value for the user mode stack (provided in rsi)
    mov rax, rsi             ; Load user mode stack pointer
    push rax
    ; Push RFLAGS with the interrupt flag enabled
    pushfq                   ; Push current flags
    or qword [rsp], 1 << 9   ; Enable interrupts in the pushed RFLAGS
    or qword [rsp], 0x202
    ; Push CS selector + RPL = 3 (Ring 3)
    push 0x18 | 3
    ; Push RIP (user mode code entry point, provided in rdi)
    mov rax, rdi             ; Load user mode code entry point
    push rax
    ; Perform the far return to transition to user mode
    iretq
