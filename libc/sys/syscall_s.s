[bits 64]
[extern syscall_handler]


[extern get_current_proc_kenrel_addr]
[extern set_current_proc_kenrel_addr]

section .data
    rsp_temp dq 0  ; Declare a 64-bit (8-byte) variable
    
section .text
[global syscall_entry]



syscall_entry:

    swapgs
    mov [rbx + 120], r11  ; Store eflags (from syscall)
    mov [rbx + 128], rcx  ; Store rip (from syscall)
    mov [rbx + 136], rsp  ; Store original rsp
    mov rdi, rsp ; Set rdi to point to the current stack (this becomes a pointer to pt_regs)

    push rax
    call get_current_proc_kenrel_addr
    mov qword [rsp_temp], rax
    pop rax
    ; mov qword [gs:8], rsp ; save user stack
    mov rsp, [rsp_temp] ; load kernel stack


    ; Prepare for calling the C handler 
    push rbx
    call syscall_handler    ; Call the C function to handle the syscall
    pop rbx


    push rdi
    mov rdi, rsp
    call set_current_proc_kenrel_addr
    pop rdi


    mov r11, [rbx + 120]  ; Store eflags (from syscall)
    mov rcx, [rbx + 128]  ; Store rip (from syscall)
    mov rsp, [rbx + 136]  ; Store original rsp

    ; mov rsp, [gs:8] ; load user stack
    ; swapgs

    ; sti
    ; Return to user space
    ; mov r11, 0x002
    o64 sysret
