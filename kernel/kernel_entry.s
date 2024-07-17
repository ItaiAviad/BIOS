; Kernel Entry Point

[bits 64]
[extern kmain]

section .entry
global _kentry
_kentry:
	call kmain
	jmp $
