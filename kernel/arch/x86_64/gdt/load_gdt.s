
[bits 64]
[global load_gdt]

; Define the GDTR structure size
gdtr_size equ 0xA  ; 2 bytes for limit + 8 bytes for base

load_gdt:
    cli
    push rbp               ; Save old base pointer
    mov rbp, rsp           ; Set up new base pointer
    sub rsp, 0x10     ; Allocate space for GDTR (10 bytes)
    
    ; Store GDTR values
    mov [rbp - gdtr_size], di      ; Store the limit (16 bits)
    mov rax, rsi
    mov [rbp - gdtr_size + 2], rax ; Store the base address (64 bits)
    
    ; Load GDTR
    lgdt [rbp - gdtr_size] ; Load GDTR from stack frame

    mov rax, 0x10
    push rax
    lea rdx, [rel reload_cs]
    push rdx
    

    jmp far [rdx]

reload_cs:
    mov rsp, rbp
    mov ax, 0x20
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax
    sti
    pop rbp
    ret