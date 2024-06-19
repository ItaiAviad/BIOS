; Bootloader
org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

start:
    jmp main

; Print string to screen
; @param ds:si - points to string
puts:
    ; save regs
    push si
    push ax
.loop:
    lodsb   ; load next char to al
    or al, al
    jz .done

    ; print char
    mov ah, 0x0e
    mov bh, 0x0
    int 0x10

    jmp .loop
.done:
    pop ax
    pop si
    ret

main:
    ; clearn interrupt flag
    cli

    ; setup data segment
    mov ax, 0
    mov ds, ax
    mov es, ax
    ; setup stack
    mov ss, ax
    mov sp, 0x7C00

    ; print boot message
    mov si, boot_msg
    call puts

    hlt

.hlt:
    jmp .hlt

boot_msg: db 'Boot successful!', ENDL, 0

times 510-($-$$) db 0
dw 0xAA55
