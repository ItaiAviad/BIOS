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
    push ax
    ; Push RSP value for the user mode stack (provided in rsi)
    mov rax, [rdi+8*17]             ; Load user mode stack pointer
    push rax
    ; Push RFLAGS with the interrupt flag enabled
    push qword [rdi+8*15]                   ; Push current flags
    or qword [rsp], 1 << 9   ; Enable interrupts in the pushed RFLAGS
    or qword [rsp], 0x202
    ; Push CS selector + RPL = 3 (Ring 3)
    push 0x18 | 3
    ; Push RIP (user mode code entry point, provided in rdi)
    mov rax, [rdi+8*16]             ; Load user mode code entry point
    push rax


    mov r15, [rdi]        ; Load r15 from the structure (rdi + 0)
    mov r14, [rdi + 8]    ; Load r14 from the structure (rdi + 8)
    mov r13, [rdi + 16]   ; Load r13 from the structure (rdi + 16)
    mov r12, [rdi + 24]   ; Load r12 from the structure (rdi + 24)
    mov rbp, [rdi + 32]   ; Load rbp from the structure (rdi + 32)
    mov rbx, [rdi + 40]   ; Load rbx from the structure (rdi + 40)

    ; Restore callee-clobbered registers (r11 to rdi)
    mov r11, [rdi + 48]   ; Load r11 from the structure (rdi + 48)
    mov r10, [rdi + 56]   ; Load r10 from the structure (rdi + 56)
    mov r9, [rdi + 64]    ; Load r9 from the structure (rdi + 64)
    mov r8, [rdi + 72]    ; Load r8 from the structure (rdi + 72)
    mov rax, [rdi + 80]   ; Load rax from the structure (rdi + 80)
    mov rcx, [rdi + 88]   ; Load rcx from the structure (rdi + 88)
    mov rdx, [rdi + 96]   ; Load rdx from the structure (rdi + 96)
    mov rsi, [rdi + 104]  ; Load rsi from the structure (rdi + 104)
    mov rdi, [rdi + 112]  ; Load rdi from the structure (rdi + 112)
    ; Perform the far return to transition to user mode
    iretq
