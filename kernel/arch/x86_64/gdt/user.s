section .text
[global jump_usermode]

jump_usermode:
    mov ax, 0x20 | 3         ; ring 3 data selector with RPL 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax               ; SS is handled by iretq

    ; Push SS selector + RPL = 3 (Ring 3)
    push 0x20 | 3
    ; Push RSP value that IRETQ will return to (the stack pointer for the user mode)
    mov rax, 0xF000   ; Ensure rsp_user_mode is set correctly for user mode stack
    push rax
    ; Push RFLAGS (including interrupt flag enabled)
    pushfq                   ; Use pushfq for 64-bit environment
    or qword [rsp], 1 << 9   ; Set interrupt flag in the pushed flags value
    ; Push CS selector + RPL = 3 (Ring 3)
    push 0x18 | 3
    ; Push RIP (address where CPU should start execution in user mode)
    mov rax, USER_LOAD_ADDR
    push rax
    ; Perform the far return to transition to user mode
    iretq