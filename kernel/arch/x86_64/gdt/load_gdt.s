
[global load_gdt]
gdtr DW 0 ; For limit storage
     DQ 0 ; For base storage

load_gdt:
    cli
    mov [gdtr], di
    mov [gdtr+2], rsi
    lgdt [gdtr]         ; Load GDT Register with LGDT 
    sub rsp, 8 

    mov ax, 0x10
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    add rsp, 8  
    ret