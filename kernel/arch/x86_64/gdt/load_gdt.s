
[global load_gdt]
gdtr DW 0 ; For limit storage
     DQ 0 ; For base storage
load_gdt:
    cli
    mov [gdtr], di
    mov [gdtr+2], rsi
    ; mov rax, [rdi]
    lgdt [gdtr]         ; Load GDT Register with GDT 

    ; mov ax, 0x10
    ; mov ds, ax         ; Reload all the data descriptors with Data selector  
    ; mov es, ax
    ; mov gs, ax
    ; mov fs, ax
    ; mov ss, ax
    ret
    ; jmp .done
                        ; Do the FAR JMP to next instruction to set CS with Code selector, and
                        ;    set the EIP (instruction pointer) to offset of setcs
.done:
	ret

[global reloadSegments]
reloadSegments:
   ; Reload CS register:
   PUSH 0x08                 ; Push code segment to stack, 0x08 is a stand-in for your code segment
   PUSH 0x08                 ; Push code segment to stack, 0x08 is a stand-in for your code segment
   LEA RAX, [rel .reload_CS] ; Load address of .reload_CS into RAX
   PUSH RAX                  ; Push this value to the stack
   RETFQ                     ; Perform a far return, RETFQ or LRETQ depending on syntax
.reload_CS:
   ; Reload data segment registers
   MOV   AX, 0x10 ; 0x10 is a stand-in for your data segment
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
   RET