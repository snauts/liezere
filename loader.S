	.area CODE
_entry:
	;; disable text
	ld	hl, #0x5cbb
	ld	(hl), #111

	;; draw line
	ld	hl, #0x4310
	ld	bc, #0x0fe0
	ld	de, #0x0800
	ld	a, #8
_line:
	ld	(hl), #4
	add	hl, de
	ld	(hl), #4
	add	hl, de
	ld	(hl), #4
	sbc	hl, bc

	dec	a
	jr	nz, _line

	;; line color
	ld	hl, #0x5810
	ld	bc, #0x0020
	ld	a, #22
_color:
	ld	(hl), #0x3d
	add	hl, bc
	dec	a
	jr	nz, _color

	ld	(hl), #0x38
	add	hl, bc
	ld	(hl), #0x3f

	;; draw mormishka
	ld	hl, #_data
	ld	de, #0x50d0
	ld	a, #8
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
