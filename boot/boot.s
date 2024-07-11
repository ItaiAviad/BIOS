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
    call puts16

    ; Read Protected Mode Sector from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (pm - _start) / sector_size ; LBA (sector address/offset)
    mov cl, (pm_end - pm) / sector_size ; # of sectors to read
    mov bx, pm ; Destination address
    call disk_read
    
    ; Print Protected Mode message
    mov si, msg_pm_sector
    call puts16

    ; Read Long Mode Sector from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (lm - _start) / sector_size ; LBA (sector address/offset)
    mov cl, (lm_end - lm) / sector_size ; # of sectors to read
    mov bx, lm ; Destination address
    call disk_read
    
    ; Print Long Mode message
    mov si, msg_lm_sector
    call puts16

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
    call clear32
    mov esi, msg_pm_switch_success ; 32bit Protected Mode success message
    call puts32

    ; Detect Long Mode support
    detect_long_mode:
        jmp .detect_cpuid
        .lm_not_supported:
            call clear32
            mov esi, msg_lm_not_supported
            call puts32
            jmp hlt
        .detect_cpuid:
            ; Detect CPUID support
            pushfd ; Push EFLAGS stack
            pop eax ; Copy EFLAGS to eax
            mov ecx, eax ; Save to ecx for comparison later
            xor eax, 1 << 21 ; Flip ID bit (21st bit of EFLAGS)
            ; Write to EFLAGS
            push eax
            popfd
            ; Read from EFLAGS again
            pushfd
            pop eax
            ; Restore EFLAGS to the older version saved in ecx
            push ecx
            popfd
            ; Compare eax and ecx (before and after)
            ; If equal then the bit got flipped back during copy -> CPUID not supported
            cmp eax, ecx
            jne .detect_cpuid_extended
            ; CPUID is supported
            call clear32
            mov esi, msg_cpuid_not_supported
            call puts32
            jmp hlt
        .detect_cpuid_extended:
            mov eax, 0x80000000    ; Set the A-register to 0x80000000.
            cpuid                  ; CPU identification.
            cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
            jb .lm_not_supported   ; It is less, there is no long mode.
        
        ; Check if Long Mode is supported
        mov eax, 0x80000001    ; Set the A-register to 0x80000001.
        cpuid                  ; CPU identification.
        test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
        jz .lm_not_supported   ; They aren't, there is no long mode.
    
    ; Elevate to Long Mode
    call elevate_lm
    ; TODO

    jmp hlt

msg_pm_switch_success: dw PREFIX, 'Protected Mode!', ENDL, 0
msg_cpuid_not_supported: dw PREFIX, 'CPUID not supported!', ENDL, 0
msg_lm_not_supported: dw PREFIX, 'LM not supported!', ENDL, 0

%include "vga_functions.s"
%include "gdt.s"
%include "A20.s"
%include "ms.s"

times (sector_size - (($-pm) % sector_size)) db 0x00
pm_end:

; -----------------------------------------------
; Long Mode Sector
; [bits 64]

lm: ; TODO
    
    jmp hlt

%include "init_paging.s"

times (sector_size - (($-lm) % sector_size)) db 0x00
lm_end:
