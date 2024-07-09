; Disk memory management (BIOS)

[bits 16]

; Convert LBA address to CHS address
; @params:
;   ax - LBA address
; @return
;   cl - sector number
;   ch - cylinder number
;   dh - head number
lba_to_chs:
    ; Save registers
    push ax
    push dx

    xor cx, cx ; cx = 0
    xor dx, dx ; dx = 0
    div word [sectors_per_track] ; ax = LBA / sectors_per_track
                                 ; dx = LBA % sectors_per_track
    inc dx ; dx = (LBA % sectors_per_track) + 1
    mov cx, dx ; cx = sector

    xor dx, dx ; dx = 0
    div word [number_heads] ; ax = (LBA / sectors_per_track) / heads = cylinder
                     ; dx = (LBA / sectors_per_track) % heads = head

    mov dh, dl ; dh = head
    mov ch, al ; ch = cylinder & 0xff
    shl ah, 6
    or cl, ah ; cl = sector | ((cylinder 0xFF00) << 6)

    pop ax
    mov dl, al ; Restore dl
    pop ax
    ret


; Disk Read Failure
disk_read_fail:
    mov si, msg_disk_read_failed
    call puts

    ; Wait for a key press
    mov ah, 0x0
    int 0x16

    jmp 0xFFFF:0 ; Jump to beginning of BIOS, should reboot


; Read sectors from disk
; @params:
;   ax - LBA address
;   cx - number of sectors (<128)
;   dl - drive number
;   es:[bx] - where to store the read data
disk_read:
    ; Save modified registers
    pop ax
    pop bx
    pop cx
    pop dx
    pop di

    push cx ; Save cx (number of sectors to read)
    call lba_to_chs
    pop ax ; al = number of sectors to read
    mov di, 3 ; retry count
.retry:
    pusha ; BIOS registers safety
    stc ; set carry flag
    ; BIOS Disk Read
    mov ah, 0x2
    int 0x13
    jnc .done ; success (carry flag cleared)

    ; read failed
    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry
.fail:
    jmp disk_read_fail
.done:
    popa
    ; Restore modified registers
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret


; Disk Reset
; @params:
;   dl - drive number
disk_reset:
    pusha
    mov ah, 0x0
    stc
    int 0x13
    jc disk_read_fail
    popa
    ret

number_heads dw 1
sectors_per_track dw 16
drive_number db 0

msg_disk_read_failed: db 'Disk Read Failed!', ENDL, 0
