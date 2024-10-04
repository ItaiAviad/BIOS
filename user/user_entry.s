; User Entry Point

[bits 64]
[extern umain]

section .entry
[global _uentry]
_uentry:
	call umain
	jmp $
