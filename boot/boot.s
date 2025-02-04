; Bootloader

[org 0x7C00]; The bios loads the first 512 bits (which are known by the name boot sector) of the device to the address 0X7C00
[bits 16]

start:
    jmp main

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
    mov si, msg_boot
    call puts

    ; Read second sectro from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, 0x1 ;: LBA = 1, second sector from disk
    mov cl, 0x1 ; Read 1 sector
    mov bx, bootsector_extended ; Read to 0x7E00

    call disk_read

    jmp bootsector_extended  
    



%include "print.s"
%include "disk.s"
%include "gdt.s"

hlt:
  cli
  jmp $

msg_boot: db 'Boot-sector successfully loaded from disk by the bios!', ENDL, 0


times 510-($-$$) db 0; This fills the space so the magic number defined below will be at addresses: [511-512]
dw 0xAA55; Magic number


bootsector_extended:; this part will be loaded from the disk

    mov si, msg_loaded_from_disk
    call puts
    jmp hlt


msg_loaded_from_disk: db 'Loaded second part of boot loader from disk', ENDL, 0
times 512 - ($ - bootsector_extended) db 0x00
