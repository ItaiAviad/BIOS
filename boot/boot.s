; Bootloader

[org 0x7C00]; The bios loads the first 512 bits (which are known by the name boot sector) of the device to the address 0X7C00

_start:
    jmp rm

; -----------------------------------------------
; Real Mode Sector
[bits 16]

rm:
    ; Disable Interrupts
    cli

    ; Setup data segment
    mov ax, 0
    mov ds, ax
    mov es, ax
    ; Setup stack
    mov ss, ax
    mov bp, 0x7C00
    mov sp, bp

    ; Print Real Mode message
    mov si, msg_rm_sector
    call puts

    ; Read Protected Mode Sector from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, 0x1 ; LBA = 1, second sector from disk
    mov cl, 0x1 ; Read 1 sector
    mov bx, pm ; Read to 0x7E00
    call disk_read
    
    ; Print Protected Mode message
    mov si, msg_pm_sector
    call puts

    ; Read Long Mode Sector from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, 0x2 ; LBA = 2, third sector from disk
    mov cl, 0x1 ; Read 1 sector
    mov bx, lm ; Read to 0x7E00
    call disk_read
    
    ; Print Long Mode message
    mov si, msg_lm_sector
    call puts

    ; Elevate to Protected Mode
    call elevate_pm
    
hlt:
  cli
  jmp $


%include "print16.s"
%include "disk.s"

msg_rm_sector: db PREFIX, 'RM Sector loaded!', ENDL, 0
msg_pm_sector: db PREFIX, 'PM Sector loaded!', ENDL, 0
msg_lm_sector: db PREFIX, 'LM Sector loaded!', ENDL, 0

times 510-($-$$) db 0; Fill up space so that Magic Number will be at addresses: [511-512]
dw 0xAA55; Magic number


; -----------------------------------------------
; Protected Mode Sector
[bits 32]

pm:
    mov eax, 0xDEAD
    jmp hlt


%include "gdt.s"
%include "A20.s"
%include "ms.s"

times 512-($-pm) db 0x00


; -----------------------------------------------
; Long Mode Sector
; [bits 64]

lm: ; TODO
    jmp hlt


times 512-($-lm) db 0x00
