; process.asm
section .text
global save_cpu_state
save_cpu_state:
    ; Input: rdi = pointer to cpu_state struct
    mov [rdi + 0], r15   ; Offset 0
    mov [rdi + 8], r14   ; Offset 8
    mov [rdi + 16], r13  ; Offset 16
    mov [rdi + 24], r12  ; Offset 24
    mov [rdi + 32], rbp  ; Offset 32
    mov [rdi + 40], rbx  ; Offset 40
    mov [rdi + 48], r11  ; Offset 48
    mov [rdi + 56], r10  ; Offset 56
    mov [rdi + 64], r9   ; Offset 64
    mov [rdi + 72], r8   ; Offset 72
    mov [rdi + 80], rax  ; Offset 80
    mov [rdi + 88], rcx  ; Offset 88
    mov [rdi + 96], rdx  ; Offset 96
    mov [rdi + 104], rsi ; Offset 104
    mov [rdi + 112], rdi ; Offset 112 - Save rdi last!
    ret