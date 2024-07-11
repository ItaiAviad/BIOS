; Bootloader

[org 0x7C00]; BIOS loads the first 512 bits (boot sector) of the device to address 0x7C00

sector_size: equ 512d

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
    mov ax, (pm - _start) / sector_size ; LBA (sector address/offset)
    mov cl, (pm_end - pm) / sector_size ; # of sectors to read
    mov bx, pm ; Destination address
    call disk_read
    
    ; Print Protected Mode message
    mov si, msg_pm_sector
    call puts

    ; Read Long Mode Sector from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (lm - _start) / sector_size ; LBA (sector address/offset)
    mov cl, (lm_end - lm) / sector_size ; # of sectors to read
    mov bx, lm ; Destination address
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
    call clear_protected
    mov esi, msg_pm_switch_success ; 32bit Protected Mode success message
    call print_protected
    call detect_lm_protected; Check if the cpu supports 64 bits
    mov esi, msg_lm_supported
    call print_protected
    call init_paging_protected


    jmp hlt


msg_pm_switch_success: dw 'Protected Mode!', ENDL, 0
msg_lm_supported: dw "This cpu supports 64 bit", ENDL, 0

%include "vga_functions.s"
%include "gdt.s"
%include "A20.s"
%include "ms.s"
%include "detect_lm.s"
%include "init_paging.s"

times (sector_size - (($-pm) % sector_size)) db 0x00
pm_end:

; -----------------------------------------------
; Long Mode Sector
; [bits 64]

lm: ; TODO
    jmp hlt


times (sector_size - (($-lm) % sector_size)) db 0x00
lm_end:
