
[bits 64]

; Clear the VGA memory. (AKA write blank spaces to every character slot)
; @parms:
;   ds:edi - style of text
clear_long:
    ; The pusha command stores the values of all
    ; registers so we don't have to worry about them
    push rdi
    push rax
    push rcx

    shl rdi, 8
    mov rax, rdi

    mov al, space_char

    mov rdi, vga_start
    mov rcx, vga_extent / 2

    rep stosw

    pop rcx
    pop rax
    pop rdi
    ret


space_char:                     equ ` `



; Print in 32 bit protected mode using vga
; @parms:
;   ds:esi - pointer to the string
;   ds:edi - style of text
print_long:
    ; The pusha command stores the values of all
    ; registers so we don't have to worry about them
    push rax
    push rdx
    push rdi
    push rsi

    mov rdx, vga_start
    shl rdi, 8

    ; Do main loop
    print_long_loop:
        ; If char == \0, string is done
        cmp byte[rsi], 0
        je  print_long_done

        ; Handle strings that are too long
        cmp rdx, vga_start + vga_extent
        je print_long_done

        ; Move character to al, style to ah
        mov rax, rdi
        mov al, byte[rsi]

        ; Print character to vga memory location
        mov word[rdx], ax

        ; Increment counter registers
        add rsi, 1
        add rdx, 2

        ; Redo loop
        jmp print_long_loop

print_long_done:
    ; Popa does the opposite of pusha, and restores all of
    ; the registers
    pop rsi
    pop rdi
    pop rdx
    pop rax

    ret
