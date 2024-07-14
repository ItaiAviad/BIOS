; Kernel Entry Point

[bits 64]
[extern kmain]

section .text
global _kentry
_kentry:
	call kmain
	jmp $
