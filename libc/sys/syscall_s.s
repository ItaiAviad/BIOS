[bits 64]
[extern syscall_handler]
    
section .text
[global syscall_entry]

%define proc_exit_syscall_num 0


syscall_entry:

    swapgs
    mov [rbx + 120], r11  ; Store eflags (from syscall)
    mov [rbx + 128], rcx  ; Store rip (from syscall)
    mov [rbx + 136], rsp  ; Store original rsp
    mov rdi, rsp ; Set rdi to point to the current stack (this becomes a pointer to pt_regs)


    mov qword [gs:8], rsp ; save user stack
    mov rsp, [gs:0] ; load kernel stack

    cmp ax, proc_exit_syscall_num

    je syscall_handler ; We don't need to add garbage to the stack if we get the exit syscall

    ; Prepare for calling the C handler 
    push rbx
    call syscall_handler    ; Call the C function to handle the syscall
    pop rbx

    mov rsp, [gs:8] ; load user stack

    mov r11, [rbx + 120]  ; Store eflags (from syscall)
    mov rcx, [rbx + 128]  ; Store rip (from syscall)
    mov rsp, [rbx + 136]  ; Store original rsp
    swapgs

    ; sti
    ; Return to user space
    mov r11, 0x002
    o64 sysret
