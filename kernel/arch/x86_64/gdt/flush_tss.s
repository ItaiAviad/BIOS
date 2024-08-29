; C declaration: void flush_tss(void);
global flush_tss
flush_tss:

	cli
	sub rsp, 8
	mov ax, 0x28
	ltr ax
	add rsp, 8
	sti
	ret
