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

; Elevate to Long mode
long_mode_msr_number: equ 0xC0000080 

[bits 32]
elevate_lm:
    call init_paging
    
    mov ecx, long_mode_msr_number ; Setting ecx to the MSR number required to switch into long mode
    rdmsr; Read the msr register
    or eax, 1 << 8; Enable 64 bit by changing the value to be written into the MSR
    wrmsr; Write to the MSR register
    
    ; Enable paging 
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    lgdt [gdt_64_descriptor]
    jmp code_seg_64:init_lm
    
[bits 64]
init_lm:
    cli
    mov ax, data_seg_64           ; Set the A-register to the data descriptor.
    mov ds, ax                    ; Set the data segment to the A-register.
    mov es, ax                    ; Set the extra segment to the A-register.
    mov fs, ax                    ; Set the F-segment to the A-register.
    mov gs, ax                    ; Set the G-segment to the A-register.
    mov ss, ax                    ; Set the stack segment to the A-register.
    jmp lm
