; Printing to screen (BIOS Printing - only works while in 16 bits real mode)

[bits 16]

; Print string to screen
; @params
;   ds:si - points to string
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


; Print hex number to screen
; @params:
;   bx - number
print_hex:
    ; Save state
    push ax
    push bx
    push cx

    mov ah, 0x0E ; Enable print mode

    ; Print prefix
    mov al, '0'
    int 0x10
    mov al, 'x'
    int 0x10

    ; Initialize cx as counter
    ; 4 nibbles in 16-bits
    mov cx, 4
    ; Begin loop
    .print_hex_bios_loop:
        ; If cx==0 goto end
        cmp cx, 0
        je .print_hex_bios_end

        push bx ; Save bx again
        shr bx, 12 ; Shift so upper four bits are lower 4 bits
        cmp bx, 10 ; Check to see if ge 10
        jge .print_hex_bios_alpha
            ; Byte in bx now < 10
            ; Set the zero char in al, add bl
            mov al, '0'
            add al, bl
            jmp .print_hex_bios_loop_end ; Jump to end of loop
        .print_hex_bios_alpha:
            ; Bit is now greater than or equal to 10
            ; Subtract 10 from bl to get add amount
            sub bl, 10   
            ; Move 'A' to al and add bl
            mov al, 'A'
            add al, bl
        .print_hex_bios_loop_end:
            int 0x10 ; Print character
            ; Restore bx
            ; Shift to next 4 bits
            pop bx
            shl bx, 4
            dec cx ; Decrement cx counter
            jmp .print_hex_bios_loop ; Jump to beginning of loop
.print_hex_bios_end:
    ; Restore state
    pop cx
    pop bx
    pop ax
    ret ; Jump to calling point


%define ENDL 0x0D, 0x0A; This defines ENDL to be a carriage return charcter followed by a line feed charcter
%define PREFIX "[*] "
