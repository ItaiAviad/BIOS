; 32bit Protected Mode VGA Functions

[bits 32]

; Print in 32 bit protected mode using vga
; @parms:
;   ds:esi - pointer to the string
puts32:
    ; The pusha command stores the values of all
    ; registers so we don't have to worry about them
    pusha
    mov edx, vga_start

    .puts32_loop:
        ; If char == \0, string is done
        cmp byte[esi], 0
        je .print32_done

        ; Move character to al, style to ah
        mov al, byte[esi]
        mov ah, style_wb

        mov word[edx], ax ; Print character to vga memory location

        ; Increment counter registers
        add esi, 1
        add edx, 2

        jmp .puts32_loop
.print32_done:
    popa
    ret


; Clear the VGA memory. (AKA write blank spaces to every character slot)
clear32:
    pusha

    ; Set up constants
    mov ebx, vga_extent
    mov ecx, vga_start
    mov edx, 0

    .clear32_loop:
        ; While edx < ebx
        cmp edx, ebx
        jge .clear32_done

        push edx ; Free edx to use later

        ; Move character to al, style to ah
        mov al, space_char
        mov ah, style_wb

        ; Print character to VGA memory
        add edx, ecx
        mov word[edx], ax

        pop edx ; Restore edx

        add edx,2 ; Restore edx

        jmp .clear32_loop
.clear32_done:
    popa
    ret



