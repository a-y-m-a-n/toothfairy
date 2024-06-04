#include <avr/io.h>

.section .data
.extern scan_line
.extern current_scan_line
.extern ddrc_next
.extern ddrd_next

; local variable of our next scan line
next_scan: .byte 1


; regs: 18, 19, 24, 30, 31 
.section .text
scanner_prep_next:

	ldi		r18, 0
	lds		r24, current_scan_line
	cpi		r24, 28					; test against highest scan line 28
	breq	next_line_index
	lds		r18, next_scan
	inc		r18

next_line_index: 
	sts		next_scan, r18
	
	/* now get the next_sense line, array index and find the next
	   sense line that needs to be asserted  */

	mov		r30, r18
	ldi		r31, 0
	subi	r30, lo8(-(scan_line))
	sbci	r31, hi8(-(scan_line))
	ld		r24, Z					; loaded the sense of the current scan 
									; to R24

	ldi		r18, 0					; ddrc_next
	ldi 	r19, 0					; ddrd_next

	/* Check for sense == 8 */
	cpi		r24, 8					; the sense line is 8 -> NOTHING to do
	brpl	save_ddrs				; nothing to do, jump to the save and return

	cpi		r24, 6					; is this a port d operation
	brpl	ddrd_op					; if the result is positive, port d

ddrc_op:
	ldi		r18, 0x1				; does not change condition code
ddrc_op_loop:
	dec		r24
	brmi	save_ddrs				; found the bit so jump and save it
	lsl		r18
	rjmp	ddrc_op_loop
	
ddrd_op:
	ldi		r19, 0x10				; does not change condition code
	subi	r24, 6					; port D is shifted by 5 bits
ddrd_op_loop:
	dec		r24
	brmi	save_ddrs
	lsl		r19
	rjmp	ddrd_op_loop
	
save_ddrs:

	/* Save the next ddrc and ddrd values */

	sts		ddrc_next, r18
	sts		ddrd_next, r19

	ret								; done return	


/*
 * The bulk of the interrupt code is identical, the only thing we do
 * different is use the scan-9 line as our index/reference pulse.  At
 * that point we know which lines are being scanned and in what order
 * so we can jump to a common interrupt handler
 */

/* INT0 Handler */
.section    .text.__vector_1,"ax",@progbits
.global __vector_1
.type   __vector_1, @function
__vector_1:

	push	r0
	in      0, _SFR_IO_ADDR(SREG)

	push 	r18
	push 	r19

	/* check to see if we are incrementing the scan */
	in		r18, _SFR_IO_ADDR(PIND)
	mov		r19, r18								; copy so we leave r18 untouched
	andi	r19, (1<<PIND2)
	brne	vector_int_common						; if PIND2 is set, then jump to common

	/* increment the scan line */
	lds		r19, current_scan_line
	inc		r19
	sts		current_scan_line, r19

	/* leave PIND in r18, we need it in a minute */
	rjmp	vector_int_common


/* INT1 Handler : Scan line 9 */
.section    .text.__vector_2,"ax",@progbits
.global __vector_2
.type   __vector_2, @function
__vector_2:

	push 	r0
	in      0, _SFR_IO_ADDR(SREG)

	push 	r18
	push 	r19

	/* check to see if we are resetting scan 9 */
	in		r18, _SFR_IO_ADDR(PIND)
	mov		r19, r18								; copy so we leave r18 untouched
	andi	r19, (1<<PIND3)
	brne	vector_int_common						; need to go on and do the update	
	
	/* Need to reset the scan line counter */
	ldi		r19, 0x9
	sts		current_scan_line, r19	

	/* leave PIND in r18, we need it in a minute */
;	rjmp	vector_int_common						; need to go on and do the update	



vector_int_common:

	/* Handle the active scan line or high-Z the system */
	/* PIND is in r18 already */

	andi	r18, (1<<PIND3)|(1<<PIND2)				; and with scan lines
	cpi		r18, (1<<PIND3)|(1<<PIND2)	
	brne	do_sense

highz:

	/* 
     * Switch DDRC and DDRD (PORTc/PORTd) to inputs, so they are high-z and
     * we are off the sense lines 
     */
	in		r18, _SFR_IO_ADDR(DDRD)
	andi	r18, ~((1<<DDD4)|(1<<DDD5))			
	out		_SFR_IO_ADDR(DDRD), r18
	in		r18, _SFR_IO_ADDR(DDRC)
	andi	r18, ~((1<<DDC5)|(1<<DDC4)|(1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0))			
	out		_SFR_IO_ADDR(DDRC), r18

	rjmp		done


/* 
 *   We make it into do_sense if we realize that we must now handle the sense lines
 *  (we are not passive and not high-z).  In this case we load up the ddrc, ddrd
 *  next values and put them on the bus.  Then we go and compute the next set of 
 *  ddrc and ddrd values (for the next scan line)
 */
do_sense:

	lds		r18, ddrc_next
	in		r19, _SFR_IO_ADDR(DDRC)
	or		r18,r19
	out		_SFR_IO_ADDR(DDRC), r18

	lds		r18, ddrd_next
	in		r19, _SFR_IO_ADDR(DDRD)
	or		r18,r19
	out		_SFR_IO_ADDR(DDRD), r18

	push	r24
	push	r25
	push 	r30
	push	r31
	rcall	scanner_prep_next						; prep next ddrc and ddrd
	pop		r31
	pop		r30
	pop		r25
	pop		r24
	
done_scan:

done:
	pop r19
	pop r18

	out     _SFR_IO_ADDR(SREG), 0
	pop r0

	
	reti											; interrupt return


