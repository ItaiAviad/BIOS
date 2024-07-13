; Entry for the kernel (Calls the main function for the kernel)
;
; needed inorder to execute the kernel only by its memory loaded address so the first instructions will start the kernel.
[bits 64]
[extern kernel_main]

section .text
global _start
_start:
    call kernel_main
    jmp $
