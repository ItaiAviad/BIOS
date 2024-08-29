
[bits 64]
[global load_gdt]

; Define the GDTR structure size
gdtr_size equ 0xA  ; 2 bytes for limit + 8 bytes for base

load_gdt:
    push rbp               ; Save old base pointer
    mov rbp, rsp           ; Set up new base pointer
    sub rsp, gdtr_size     ; Allocate space for GDTR (10 bytes)
    
    ; Store GDTR values
    mov [rbp - gdtr_size], di      ; Store the limit (16 bits)
    mov [rbp - gdtr_size + 2], rsi ; Store the base address (64 bits)
    
    ; Load GDTR
    lgdt [rbp - gdtr_size] ; Load GDTR from stack frame
    add rsp, gdtr_size
    push 0x10
    lea rax, [rel reload_cs]
    ;sub sp, 8
    ;mov [rsp], rax
    push rax
    lretq 

reload_cs:
    add rsp, 0x10

    mov ax, 0x20
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax
    
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax
    pop rbp
    sti
    ret