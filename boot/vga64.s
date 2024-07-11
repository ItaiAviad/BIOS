; 64bit Long Mode VGA Functions

[bits 64]

; Clear the VGA memory. (AKA write blank spaces to every character slot)
; @parms:
;   ds:edi - style of text
clear64:
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

; Print in 64bit Long Mode using vga
; @parms:
;   ds:esi - pointer to the string
;   ds:edi - style of text
puts64:
    push rdi
    push rax
    push rcx

    mov rdx, vga_start
    shl rdi, 8
    .puts64_loop:
        ; If char == \0, string is done
        cmp byte[rsi], 0
        je .puts64_done

        ; Handle strings that are too long
        cmp rdx, vga_start + vga_extent
        je .puts64_done

        ; Move character to al, style to ah
        mov rax, rdi
        mov al, byte[rsi]

        mov word[rdx], ax ; Print character to vga memory location

        ; Increment counter registers
        add rsi, 1
        add rdx, 2

        jmp .puts64_loop
.puts64_done:
    pop rcx
    pop rax
    pop rdi
    ret
