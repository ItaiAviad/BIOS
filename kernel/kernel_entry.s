; Kernel Entry Point

[bits 64]
[extern kmain]

global _kentry
_kentry:
	call kmain
	jmp $
