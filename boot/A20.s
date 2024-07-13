; Enable the A20 address line (via BIOS function)
;
; The A20 address line is turned off by default (for compatibility with older platforms) and to be able to use higher addresses it must be turned on. 

[bits 16]

enable_a20:
    mov     ax, 0x2403      ; --- A20-Gate Support ---
    int     0x15
    jb      a20_ns          ; INT 15h is not supported
    cmp     ah, 0x0
    jnz     a20_ns          ; INT 15h is not supported

    mov     ax, 0x2402      ; --- A20-Gate Status ---
    int     0x15
    jb      a20_failed      ; Couldn't get status
    cmp     ah, 0x0
    jnz     a20_failed      ; Couldn't get status

    cmp     al, 0x1
    jz      a20_activated   ; A20 is already activated

    mov     ax, 0x2401      ; --- A20-Gate Activate ---
    int     0x15
    jb      a20_failed      ; Couldn't activate the gate
    cmp     ah, 0x0
    jnz     a20_failed      ; Couldn't activate the gate

a20_activated:
    ret

a20_ns:
    mov si, msg_a20_ns
    call puts16
    jmp hlt

a20_failed:
    mov si, msg_a20_failed
    call puts16
    jmp hlt


msg_a20_ns: db "A20 failed, INT 0x15 is not supported", ENDL, 0
msg_a20_failed: db "A20 failed", ENDL, 0

