;    C startup routine, one device. Watcom register calling convention
;

_DATA	segment word public use16 'DATA'
_DATA	ends

CONST	segment word public use16 'CONST'
CONST	ends

_BSS	segment word public use16 'BSS'
_BSS	ends

DGROUP	group CONST,_BSS,_DATA

include devhlp.equ

		extrn   strategy_handler_:near
		extrn   timer_handler_:near
        extrn   _devhlp:dword
		public	strat_
		public  tim_hndlr_
        public  empty_intr_
		public	_cstart_

_TEXT	segment word public use16 'CODE'
 	assume cs:_TEXT,ds:DGROUP,es:NOTHING,ss:NOTHING
	.386P
;

strat_	proc	far
_cstart_:			;no startup code
;
    push    es          ;segment registers are not necessarily saved by Watcom
	mov	dx, es		;send Request Packet address
	mov     ax, bx
	call	strategy_handler_		;call driver strategy
    pop     es
	mov	word ptr es:[bx+3],ax ;send completion status
	ret
strat_	endp

;
;
;
empty_intr_ proc    far
    mov     al, 12d
    mov     dl, DevHlp_EOI
    call    _devhlp
	ret			
empty_intr_	endp


tim_hndlr_	proc    far
	pusha
	push	es
	push	ds
	call	timer_handler_
	pop	ds
	pop	es
	popa
	ret
tim_hndlr_ 	endp

_TEXT	ends
	end


