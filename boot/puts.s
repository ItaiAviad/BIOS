; Print string to screen (only works while in 16 bits real mode)
; @param ds:si - points to string

puts:
    ; save regs
    push si
    push ax
.loop:
    lodsb   ; load next char to al
    or al, al
    jz .done

    ; print char
    mov ah, 0x0e
    mov bh, 0x0
    int 0x10

    jmp .loop
.done:
    pop ax
    pop si
    ret

