; Bootloader

[org 0x7C00]; The bios loads the first 512 bits (which are known by the name boot sector) of the device to the address 0X7C00
[bits 16]

%define ENDL 0x0D, 0x0A; This defines ENDL to be a carriage return charcter followed by a line feed charcter

start:
    jmp main
  

%include "print.s"


main:
    ; clearn interrupt flag
    cli

    ; setup data segment
    mov ax, 0
    mov ds, ax
    mov es, ax
    ; setup stack
    mov ss, ax
    mov bp, 0x7C00
    mov sp, bp

    ; print boot message
    mov si, boot_msg
    call puts

    ; print hex
    mov bx, 0xABCD
    call print_hex

    hlt

.hlt:
    jmp .hlt

boot_msg: db 'Boot-sector successfully loaded from disk by the bios!', ENDL, 0

times 510-($-$$) db 0; This fills the space so the magic number defined below will be at addresses: [511-512]
dw 0xAA55; Magic number
