; Bootloader
; The SECTOR_SIZE, KERNEL_SIZE_IN_SECTORS, KERNEL_LOAD_ADDR are set in assembly time and provided to nasm via the -D option

[org 0x7C00] ; BIOS loads the first 512 bits (boot sector) of the device to address 0x7C00

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
    mov bp, KERNEL_LOAD_ADDR
    mov sp, bp

    ; Print Real Mode message
    ; mov si, msg_rm_sector
    ; call puts16

    ; Read Protected Mode Sectors from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (pm - _start) / SECTOR_SIZE ; LBA (sector address/offset)
    mov cl, (pm_end - pm) / SECTOR_SIZE ; # of sectors to read
    mov bx, pm ; Destination address
    call disk_read
    
    ; Print Protected Mode Sectors message
    ; mov si, msg_pm_sector
    ; call puts16

    ; Read Long Mode Sectors from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (lm - _start) / SECTOR_SIZE ; LBA (sector address/offset)
    mov cl, (lm_end - lm) / SECTOR_SIZE ; # of sectors to read
    mov bx, lm ; Destination address
    call disk_read

    ; Print Long Mode Sector message
    ; mov si, msg_lm_sector
    ; call puts16

    ; Read Kernel Sectors from disk
    mov [drive_number], dl ; BIOS should set dl to drive number
    mov ax, (bootloader_end - _start) / SECTOR_SIZE ; LBA (sector address/offset)
    mov cl, KERNEL_SIZE_IN_SECTORS ; # of sectors to read
    mov bx, (KERNEL_LOAD_ADDR) ; Destination address
    call disk_read

    ;Print Kernel Sectors message
    ; mov si, msg_kernel_sector
    ; call puts16

    ; Print Initializing Mode Switching message
    ; mov si, msg_init_ms
    ; call puts16

    ; Elevate to Protected Mode
    call elevate_pm
    
hlt:
  cli
  jmp $


; %include "print16.s"
%include "disk.s"
%include "gdt32.s"

; msg_rm_sector: db PREFIX, 'RM Sector loaded!', ENDL, 0
; msg_pm_sector: db PREFIX, 'PM Sector loaded!', ENDL, 0
; msg_lm_sector: db PREFIX, 'LM Sector loaded!', ENDL, 0
; msg_kernel_sector: db PREFIX, 'Kernel Sector loaded!', ENDL, 0
; msg_init_ms: db PREFIX, 'Initializing Mode Switching...', ENDL, 0

times 510-($-$$) db 0; Fill up space so that Magic Number will be at addresses: [511-512]
dw 0xAA55; Magic number


; -----------------------------------------------
; Protected Mode Sector
[bits 32]

pm:
    ; call clear32
    ; mov esi, msg_pm_success ; 32bit Protected Mode success message
    ; call puts32

    ; Detect Long Mode support
    detect_long_mode:
        jmp .detect_cpuid
        .lm_not_supported:
            ; call clear32
            ; mov esi, msg_lm_not_supported
            ; call puts32
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
            ; call clear32
            ; mov esi, msg_cpuid_not_supported
            ; call puts32
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

; Constants
space_char:   equ ` `
vga_start:    equ 0x000B8000
vga_extent:   equ 80 * 25 * 2 ; VGA Memory is 80 chars wide by 25 chars tall (one char is 2 bytes)
style_wb:     equ 0x0A
style_blue:   equ 0x1F

; msg_pm_success: dw PREFIX, '32bit Protected Mode!', ENDL, 0
; msg_cpuid_not_supported: dw PREFIX, 'CPUID not supported!', ENDL, 0
; msg_lm_not_supported: dw PREFIX, 'LM not supported!', ENDL, 0

; %include "vga32.s"
%include "A20.s"
%include "ms.s"

times (SECTOR_SIZE - (($-pm) % SECTOR_SIZE)) db 0x00
pm_end:

; -----------------------------------------------
; Long Mode Sector
[bits 64]

lm:
    mov rdi, style_blue
    ; mov rsi, msg_lm_success
    ; call puts64
    ; xchg bx, bx
    ; IMPORTANT: Use `jmp` and not `call` as `call` misaligns RSP
    mov sp, KERNEL_LOAD_ADDR
    mov bp, sp
    jmp KERNEL_LOAD_ADDR
    jmp hlt

; Constants
; msg_lm_success: db PREFIX, "64bit Long Mode!", 0

%include "init_paging.s"
; %include "vga64.s"
%include "gdt64.s"

times (SECTOR_SIZE - (($-lm) % SECTOR_SIZE)) db 0x00
lm_end:
bootloader_end:
