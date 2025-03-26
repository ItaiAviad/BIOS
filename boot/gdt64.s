; GDT (Global Descriptor Table) 64bit - Flat Mode Setup
; (The Flat Mode GDT table allows us to read and write code anywhere, without restriction)

; References:
; GDT - https://wiki.osdev.org/Global_Descriptor_Table
; GDT Tutorial - https://wiki.osdev.org/GDT_Tutorial

align 16

section .bss
align 16
tss:
    resq 1               ; Reserved
    resq 1               ; RSP0
    resq 1               ; RSP1
    resq 1               ; RSP2
    resq 1               ; Reserved
    resq 1               ; IST1
    resq 1               ; IST2
    resq 1               ; IST3
    resq 1               ; IST4
    resq 1               ; IST5
    resq 1               ; IST6
    resq 1               ; IST7
    resq 1               ; Reserved
    ; dw 0x0000            ; Reserved
    resw 1               ; Reserved
    ; dw 0x0000            ; IO Map Base Address
    resw 1               ; IO Map Base Address

; section .data
; align 16
; tss:
;     dq 0x0               ; Reserved
;     dq 0x0               ; RSP0
;     dq 0x0               ; RSP1
;     dq 0x0               ; RSP2
;     dq 0x0               ; Reserved
;     dq 0x0               ; IST1
;     dq 0x0               ; IST2
;     dq 0x0               ; IST3
;     dq 0x0               ; IST4
;     dq 0x0               ; IST5
;     dq 0x0               ; IST6
;     dq 0x0               ; IST7
;     dq 0x0               ; Reserved
;     ; dw 0x0000            ; Reserved
;     dw 0x0               ; Reserved
;     ; dw 0x0000            ; IO Map Base Address
;     dw 0x0               ; IO Map B

section .text
rsp0_value: dd KERNEL_VBASE
[global tss_init]
tss_init_rsp0:
    mov eax, [rsp0_value]
    mov [tss + 4], eax          ; Store RSP0 value in TSS (offset 4)
    ret
    ; mov word [tss + 4],  rsp0_value  ; Initialize RSP0
    ; mov qword [tss + 12], rsp1_value  ; Initialize RSP1
    ; mov qword [tss + 20], rsp2_value  ; Initialize RSP2
    ; mov qword [tss + 40], ist1_value  ; Initialize IST1
    ; mov qword [tss + 48], ist2_value  ; Initialize IST2

section .text

[global tss_init_gdt64]
tss_init_gdt64:
    mov eax, tss
    mov [tss_entry + 2], ax     ; Base 0-15
    shr eax, 16
    mov [tss_entry + 4], al     ; Base 16-23
    shr eax, 8
    mov [tss_entry + 7], al     ; Base 24-31
    shr eax, 8
    mov [tss_entry + 8], eax    ; Base 32-63
    ret

align 0x40
gdt64_start:; 1-7
    dd 0x0
    dd 0x0

gdt64_kcode:;8-15
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
    dw 0x0000           ; Base  (bits 16-31)
    db 0x00             ; Base  (bits 31-39)
    db 0b10011010       ; 1st Flags, Type flags (40-47)
    db 0b10101111       ; 2nd Flags, Limit (bits 48-55)
    db 0x00             ; Base  (bits 56-63)


gdt64_kdata:;16-23
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


gdt64_ucode:;24-31
    ; Base:                 0x00000
    ; Limit:                0xFFFFF
    ; 1st Flags:            0b1001
    ;   P - Present:        1
    ;   DPL - Privelege:    11
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
    db 0b11111010       ; 1st Flags, Type flags
    db 0b10101111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)


gdt64_udata:;32-39
    ; Base:                 0x00000
    ; Limit:                0x00000
    ; 1st Flags:            0b1001
    ;   P - Present:        1
    ;   DPL - Privelege:    11
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

    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b11110010       ; 1st Flags, Type flags
    db 0b10101111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

tss_entry:
    dw 0x0067                   ; Limit (104 bytes - 1)
    dw 0                        ; Base 0-15 (set by tss_init_gdt64)
    db 0                        ; Base 16-23
    db 0b10001001               ; Type: TSS available, DPL=0
    db 0b00000000               ; Flags, Limit 16-19
    db 0                        ; Base 24-31
    dd 0                        ; Base 32-63
    dw 0x0000
    dw 0x0000

gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1        ; Size of GDT, one byte less than true size
    dd gdt64_start                         ; Start of the 64 bit gdt

KCODE_SEG64: equ gdt64_kcode - gdt64_start
KDATA_SEG64: equ gdt64_kdata - gdt64_start

tss_base_low: dw 0
tss_base_mid: db 0
tss_base_high: db 0
tss_base_upper: dd 0

