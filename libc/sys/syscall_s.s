section .text
[global syscall_entry]
[extern syscall_handler]

syscall_entry:
    ; Save all general-purpose registers
    push r15                ; Save r15
    push r14                ; Save r14
    push r13                ; Save r13
    push r12                ; Save r12
    push rbp                ; Save rbp (base pointer)
    push rbx                ; Save rbx (base register)
    push r11                ; Save r11 (saved by `syscall` instruction)
    push r10                ; Save r10
    push r9                 ; Save r9
    push r8                 ; Save r8
    push rax                ; Save rax (syscall number)
    push rcx                ; Save rcx (saved by `syscall` instruction)
    push rdx                ; Save rdx (third argument)
    push rsi                ; Save rsi (second argument)
    push rdi                ; Save rdi (first argument)
    
    ; push rip
    ; ; Save CS
    ; xor rax, rax
    ; mov ax, cs
    ; push rax
    ; ; Save flags
    ; pushfq                  ; Save rflags to the stack
    ; push rsp
    ; ; Save SS
    ; xor rax, rax
    ; mov ax, ss
    ; push rax

    ; Prepare for calling the C handler
    mov rdi, rsp            ; Set rdi to point to the current stack (this becomes a pointer to pt_regs)
    call syscall_handler    ; Call the C function to handle the syscall

    ; Restore all registers (reverse order)
    ; pop ss                  ; Restore SS
    ; pop rsp                 ; Restore RSP
    ; pop eflags              ; Restore EFLAGS
    ; pop cs                  ; Restore CS
    ; pop rip                 ; Restore RIP
    ; popfq                   ; Restore flags
    pop rdi                 ; Restore rdi
    pop rsi                 ; Restore rsi
    pop rdx                 ; Restore rdx
    pop rcx                 ; Restore rcx
    pop rax                 ; Restore rax
    pop r8                  ; Restore r8
    pop r9                  ; Restore r9
    pop r10                 ; Restore r10
    pop r11                 ; Restore r11
    pop rbx                 ; Restore rbx
    pop rbp                 ; Restore rbp
    pop r12                 ; Restore r12
    pop r13                 ; Restore r13
    pop r14                 ; Restore r14
    pop r15                 ; Restore r15

    ; Return to user space
    sysretq
