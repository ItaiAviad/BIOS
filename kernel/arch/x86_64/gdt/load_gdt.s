
[global load_gdt]
load_gdt:
    cli
    mov rax, [rsp+8]
    lgdt [rax]         ; Load GDT Register with GDT 
    mov ax, 0x10
    mov ds, ax         ; Reload all the data descriptors with Data selector  
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    jmp .done
                        ; Do the FAR JMP to next instruction to set CS with Code selector, and
                        ;    set the EIP (instruction pointer) to offset of setcs
.done:
	ret