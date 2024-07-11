; Mode Switching (Real Mode -> Protected Mode -> Long Mode)

; Source: https://wiki.osdev.org/Protected_Mode

[bits 16]

; Elevate to Protected Mode
elevate_pm:
    cli ; Disable Interrupts
    call enable_a20 ; Enable A20
    lgdt [gdt_32_descriptor] ; Load GDT
    ; Enable Protected Mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm ; Far jump to protected mode (to clean the pipline from previous 16bit commands)

    [bits 32]
    init_pm:
        mov ax, DATA_SEG
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        ; Set up stack
        mov ebp, 0x90000
        mov esp, ebp

        jmp pm

; Elevate to Long Mode - TODO
elevate_lm:
    ; TODO
    init_paging
    
    init_lm:
        

