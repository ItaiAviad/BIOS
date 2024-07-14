; Kernel Entry Point

[bits 64]
[extern kmain]

section .text
global _kstart
_kstart:
	call kmain
	jmp $
