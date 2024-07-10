[bits 32]

; Print in 32 bit protected mode using vga
; @parms
;   ds:esi - pointer to the string
print_protected:
    ; The pusha command stores the values of all
    ; registers so we don't have to worry about them
    pusha
    mov edx, vga_start

    ; Do main loop
    print_protected_loop:
        ; If char == \0, string is done
        cmp byte[esi], 0
        je  print_protected_done

        ; Move character to al, style to ah
        mov al, byte[esi]
        mov ah, style_wb

        ; Print character to vga memory location
        mov word[edx], ax

        ; Increment counter registers
        add esi, 1
        add edx, 2

        ; Redo loop
        jmp print_protected_loop

print_protected_done:
    ; Popa does the opposite of pusha, and restores all of
    ; the registers
    popa
    ret


; Clear the VGA memory. (AKA write blank spaces to every character slot)
; @parms
;
clear_protected:
    ; The pusha command stores the values of all
    ; registers so we don't have to worry about them
    pusha

    ; Set up constants
    mov ebx, vga_extent
    mov ecx, vga_start
    mov edx, 0

    ; Do main loop
    clear_protected_loop:
        ; While edx < ebx
        cmp edx, ebx
        jge clear_protected_done

        ; Free edx to use later
        push edx

        ; Move character to al, style to ah
        mov al, space_char
        mov ah, style_wb

        ; Print character to VGA memory
        add edx, ecx
        mov word[edx], ax

        ; Restore edx
        pop edx

        ; Increment counter
        add edx,2

        ; GOTO beginning of loop
        jmp clear_protected_loop

clear_protected_done:
    ; Restore all registers and return
    popa
    ret


space_char:                     equ ` `

; Define necessary constants
vga_start:                  equ 0x000B8000
vga_extent:                 equ 80 * 25 * 2             ; VGA Memory is 80 chars wide by 25 chars tall (one char is 2 bytes)
style_wb:                   equ 0x0F
