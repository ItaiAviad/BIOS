; C declaration: void flush_tss(void);
global flush_tss
flush_tss:
	; mov ax, (5 * 8) | 0 ; fifth 8-byte selector, symbolically OR-ed with 0 to set the RPL (requested privilege level).
	mov ax, 0x28
	ltr ax
	ret
