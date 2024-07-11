; GDT (Global Descriptor Table) 64bit

; References:
; GDT - https://wiki.osdev.org/Global_Descriptor_Table
; GDT Tutorial - https://wiki.osdev.org/GDT_Tutorial

align 4

gdt64_start:
    dd 0x0
    dd 0x0

gdt64_code:
    ; Base:                 0x00000
    ; Limit:                0xFFFFF
    ; 1st Flags:            0b1001
    ;   P - Present:        1
    ;   DPL - Privelege:    00
    ;   S - Descriptor:     1
    ; Type Flags:           0b1010
    ;   E - Code:           1
    ;   DC - Conforming:    0
    ;   RW - Readable:      1
    ;   A - Accessed:       0
    ; 2nd Flags:            0b1100
    ;   G - Granularity:    1
    ;   DB - 32bit Default: 0
    ;   L - 64bit Segment:  1
    ;   Reserved - AVL:     0

    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10011010       ; 1st Flags, Type flags
    db 0b10101111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)


gdt64_data:
    ; Base:                 0x00000
    ; Limit:                0x00000
    ; 1st Flags:            0b1001
    ;   P - Present:        1
    ;   DPL - Privelege:    00
    ;   S - Descriptor:     1
    ; Type Flags:           0b0010
    ;   E - Code:           0
    ;   DC - Expand Down:   0
    ;   RW - Writeable:     1
    ;   A - Accessed:       0
    ; 2nd Flags:            0b1100
    ;   G - Granularity:    1
    ;   DB - 32bit Default: 0
    ;   L - 64bit Segment:  1
    ;   Reserved - AVL:     0

    dw 0x0000           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10010010       ; 1st Flags, Type flags
    db 0b10100000       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1        ; Size of GDT, one byte less than true size
    dd gdt64_start                         ; Start of the 64 bit gdt


CODE_SEG64: equ gdt64_code - gdt64_start
DATA_SEG64: equ gdt64_data - gdt64_start
