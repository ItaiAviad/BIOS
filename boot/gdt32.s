; GDT (Global Descriptor Table) 32bit - Flat Mode Setup
; (The Flat Mode GDT table allows us to read and write code anywhere, without restriction)

; References:
; GDT - https://wiki.osdev.org/Global_Descriptor_Table
; GDT Tutorial - https://wiki.osdev.org/GDT_Tutorial

gdt32_start:

; Null Descriptor - required for memory integrity check
gdt32_null:
    dd 0x00000000
    dd 0x00000000

; Kernel Mode Code Segment
gdt32_code:
    ; Base:                 0x00000
    ; Limit:                0xFFFFF
    ; 1st Flags:            0b1001 (bits 4-7)
    ;   P - Present:        1
    ;   DPL - Privelege:    00
    ;   S - Descriptor:     1
    ; Type Flags:           0b1010 (bits 0-4)
    ;   E - Code:           1
    ;   DC - Conforming:    0
    ;   RW - Readable:      1
    ;   A - Accessed:       0
    ; 2nd Flags:            0b1100
    ;   G - Granularity:    1
    ;   DB - 32bit Mode:    1
    ;   L - 64bit Segment:  0
    ;   Reserved - AVL:     0

    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10011010       ; 1st Flags, Type flags
    db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

; Kernel Mode Data Segment
gdt32_data:
    ; Base:                 0x00000
    ; Limit:                0xFFFFF
    ; 1st Flags:            0b1001 (bits 4-7)
    ;   P - Present:        1
    ;   DPL - Privelege:    00
    ;   S - Descriptor:     1
    ; Type Flags:           0b0010 (bits 0-4)
    ;   E - Code:           0
    ;   DC - Expand Down:   0
    ;   RW - Writeable:     1
    ;   A - Accessed:       0
    ; 2nd Flags:            0b1100
    ;   G - Granularity:    1
    ;   DB - 32bit Mode:    1
    ;   L - 64bit Segment:  0
    ;   Reserved - AVL:     0

    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10010010       ; 1st Flags, Type flags
    db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

gdt32_end:

; GDT Descriptor
; Gives CPU: length and start address of GDT (to set the Protected Mode GDT)
gdt32_descriptor:
    dw gdt32_end - gdt32_start - 1        ; Size of GDT, one byte less than true size
    dd gdt32_start                         ; Start of the 32 bit GDT

; Code and Data segments' offsets
CODE_SEG32: equ gdt32_code - gdt32_start
DATA_SEG32: equ gdt32_data - gdt32_start