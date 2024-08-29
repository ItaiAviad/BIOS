; Mode Switching (Real Mode -> Protected Mode -> Long Mode)

; Source: https://wiki.osdev.org/Protected_Mode
; Source: https://wiki.osdev.org/X86-64
; Source: https://wiki.osdev.org/Setting_Up_Long_Mode

[bits 16]

; Elevate to Protected Mode
elevate_pm:
    cli ; Disable Interrupts

    call enable_a20 ; Enable A20

    ; Load 32bit GDT
    lgdt [gdt32_descriptor]

    ; Enable Protected Mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to Protected Mode (to clean the pipline from previous 16bit commands)
    jmp CODE_SEG32:init_pm

    [bits 32]
    init_pm:
        mov ax, DATA_SEG32
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        ; Set up stack
        mov ebp, KERNEL_LOAD_ADDR
        mov esp, ebp

        jmp pm

; Elevate to Long mode
[bits 32]
elevate_lm:

    ; Set up paging
    call init_paging
    ; xchg bx, bx
    ; Set LME bit in MSR (EFER) (Long Mode Enable)
    mov ecx, 0xC0000080
    rdmsr ; Read MSR specified by ecx into edx:eax
    or eax, 1 << 8 ; Enable LME bit in MSR register
    wrmsr ; Write the value in edx:eax to MSR specified by ecx
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Setup TSS addresses
    call tss_init_rsp0
    call tss_init_gdt64
    
    ; Load 64bit GDT
    lgdt [gdt64_descriptor]

    ; Far jump to Long Mode
    jmp KCODE_SEG64:init_lm
    
    [bits 64]
    init_lm:
        cli ; Disable Interrupts

        ; Set up segment registers
        mov ax, KDATA_SEG64
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        
        sti
        jmp lm
