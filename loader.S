	.area LOADER
_entry:
	ld	hl, #0x8000
	ld	(hl), #0xc9
	call	0x8000

_near:
	dec	sp
	dec	sp
	pop	hl

	ld	de, #_data - #_near
	add	hl, de

	ld	de, #0x50de
	ld	a, #0xb

_loop:
	ldi
	dec	e
	inc	d
	dec	a
	jr	nz, _loop

	ret

_data:
	.db	#0x06
	.db	#0x0f
	.db	#0x0b
	.db	#0x0f
	.db	#0x4e
	.db	#0x8c
	.db	#0x98
	.db	#0x70
	.db	#0x3f
	.db	#0x3f
	.db	#0x38
