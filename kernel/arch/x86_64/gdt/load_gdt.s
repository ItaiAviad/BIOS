
[bits 64]
[global load_gdt]
gdtr DW 0 ; For limit storage
     DQ 0 ; For base storage

load_gdt:
    cli
    mov [gdtr], di
    mov [gdtr+2], rsi
    lgdt [gdtr]         ; Load GDT Register with LGDT
    push 0x10
    lea rax, [rel reload_cs]
    ;sub sp, 8
    ;mov [rsp], rax
    push rax
    lretq 

reload_cs:
    mov ax, 0x20
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax
    add sp, 0x10
    sti
    ret