MAIN:	add r3, LIST
LOOP:	prn	#48
	macr mmacr
	cmp r3, #-6
	bne END
	endmacr
	lea STR, r6
	inc r6
	mov *r6,K
	sub r1, r4
	mmacr
	dec K
	jmp LOOP
END:	stop
STR:	.string "abcd"
LIST:	.data 6, -9
	.data -100
K:	.data 31
	.entry test
	.entry END
