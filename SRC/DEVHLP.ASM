;   devhlp.asm
;
;   devhlp calls c-wrapper for Watcom C. Should be faster than IBMs dhcalls version (better parameter passing)
;
;   Copyright (c) 1995, 1996 Immisch, Becker & Partner, Hamburg & Lars Immisch
;
;   Author: Lars Immisch <lars@ibp.de>

include devhlp.equ
        extrn   _devhlp:dword

        public  EOI_
        public  SetIRQ_
        public  GetDOSVar_
        public  Block_
        Public  Run_
        public  Yield_
        public  VerifyAccess_
        public  Lock_
        public  Unlock_
        public  AllocGDTSelector_
        public  PhysToGDTSelector_
        public  PhysToVirt_
        public  VirtToPhys_
        public  PhysToUVirt_
        public  AllocPhys_
        public  FreePhys_
        public  VirtToLin_
        public  Limit_
        public  SendEvent_
        public  InternalError_
        public  VMAlloc_
        public  VMFree_

_TEXT	segment word public use16 'CODE'
 	assume cs:_TEXT,ds:DGROUP,es:NOTHING,ss:NOTHING
	.386P

;
;
; FARPOINTER GetDOSVar (USHORT index);
; #pragma aux GetDOSVar parm [ax]

GetDOSVar_  proc    near     ;index is in ax
;
    push    bx
    mov     dl, DevHlp_GetDOSVar    
    call    _devhlp
    jc      getdosvarerror
    mov     dx, ax
    mov     ax, bx
    pop     bx
    ret
getdosvarerror:
    pop     bx
    xor     ax, ax
    xor     dx, dx
    ret
GetDOSVar_  endp

;
; USHORT SetIRQ  (PFUNCTION handler, USHORT isShared, USHORT interrupt);
;
SETIRQ_  proc    near     ;handler is in ax, isShared is in dx, interrupt is in bx
    mov     dh, dl
    mov     dl, DevHlp_SetIRQ
    call    _devhlp
    jc      setirqerror
    xor     ax, ax
    ret
setirqerror:
    ret
SETIRQ_  endp
;
;
; USHORT EOI  (USHORT);
; #pragma aux EOI parm [ax]
;
EOI_  proc    near     ;interrupt is in ax
    push    dx        ;save dx
    mov     dl, DevHlp_EOI
    call    _devhlp
    pop     dx
    ret
;
EOI_  endp

;
; USHORT Block (FARPOINTER blockId, ULONG timeout, USHORT interruptible); 
; #pragma aux Block parm [ax bx] [cx di] [dx]
;
Block_  proc    near     ; blockId is in bx:ax, timeout is in cx:di, interruptible is in dx
    xchg    cx, di       ; Watcom refuses to pass an ULONG in DI:CX, which we need here
    xchg    ax, bx
    mov     dh, dl
    mov     dl, DevHlp_Block
    call    _devhlp
    jnc     waitnormal
    jz      waittimeout
    mov     ax, 2       ; carry is set and zero is clear: interrupted!
    ret
waittimeout:
    mov     ax, 3
    ret
waitnormal:
    xor     ax, ax
    ret
Block_  endp

;
; USHORT Run (FARPOINTER blockId);
; #pragma aux Run parm [ax bx] 
;
Run_  proc    near       ; blockId is in bx:ax
    xchg    ax, bx
    push    dx
    mov     dl, DevHlp_Run
    call    _devhlp
    pop     dx
    ret
Run_  endp

;
; VOID Yield ();
;
Yield_  proc    near
    push    dx
    mov     dl, DevHlp_Yield
    call    _devhlp
    pop     dx
    ret
Yield_  endp

;
; USHORT VerifyAccess(FPFARPOINTER anAddress, USHORT length, USHORT write)
; #pragma aux VerifyAccess parm [di ax] [cx] [dx]
;
VerifyAccess_   proc    near
    xchg    ax, di      ; Watcom refuses to pass a PVOID in AX:DI, which we need here
    mov     dh, dl
    mov     dl, DevHlp_VerifyAccess
    call    _devhlp
    xor     ax, ax      
    adc     ax, 0
    ret
VerifyAccess_   endp

;
; LHANDLE Lock(SEL sel, USHORT wait, USHORT flags); 
; #pragma aux Lock parm [ax] [bx] [dx]
;
Lock_   proc    near         ;
    mov     bh, dl
    mov     dl, DevHlp_Lock
    call    _devhlp
    jc      lockerror
    mov     dx, ax
    mov     ax, bx
    ret
lockerror:
    xor     dx, dx
    xor     ax, ax
    ret
Lock_   endp

;
; USHORT Unlock(LHANDLE)
; #pragma aux Unlock parm [ax bx] 
;
Unlock_     proc    near
    push    dx
    xchg    ax, bx          ; handle is in bx:ax
    mov     dl, DevHlp_Unlock
    call    _devhlp
    jc      unlockend       ; if error - value in ax
    xor     ax, ax          ; else clear
unlockend:
    pop     dx
    ret

Unlock_      endp

;
; USHORT AllocGDTSelector(FPUSHORT selectorArray, USHORT count);
; #pragma aux AllocGDTSelector parm [es di] [cx]
;
AllocGDTSelector_   proc    near
    push    dx
    mov     dl, DevHlp_AllocGDTSelector
    call    _devhlp
    jc      allocgdtend
    xor     ax, ax
allocgdtend:
    pop     dx
    ret
AllocGDTSelector_   endp

;
; USHORT PhysToGDTSelector(FARPOINTER phys, USHORT length, SEL selector);
; #pragma aux PhysToGDTSelector parm [ax bx] [cx] [si]
;
PhysToGDTSelector_  proc    near
    push    dx
    xchg    ax, bx
    mov     dl, DevHlp_PhysToGDTSelector
    call    _devhlp
    jc      phystogdtselectorend
    xor     ax, ax
phystogdtselectorend:
    pop     dx
    ret
PhysToGDTSelector_  endp
;
; FARPOINTER    VirtToPhys(FARPOINTER virt);
; #pragma aux VirtToPhys parm [ax bx] 
;
VirtToPhys_     proc    near
    push    es              ; dx is not saved since the result is passed in dx
    push    si
    push    ds
    push    ds
    pop     es              ; load ds into es - mov is forbidden
    mov     ds, bx          ; for reasons I don't understand: arg is in bx:ax
    mov     si, ax
    mov     dl, DevHlp_VirtToPhys
    call    es:[_devhlp]      ; we are in deep shit here: ds is not the data segment of _devhlp
    pop     ds
    pop     si
    pop     es
    jc      virttophyserror
    mov     dx, ax
    mov     ax, bx
    ret
virttophyserror:
    xor     dx, dx            ; return NULL pointer
    xor     ax, ax
    ret
VirtToPhys_      endp

;
; FARPOINTER    PhysToVirt(FARPOINTER phys);
; #pragma aux PhysToVirt parm [ax bx]
;
PhysToVirt_     proc    near    ; dx is not saved since the result is passed in dx
    push    ds
    push    si
    push    es
    xchg    ax, bx              ; arg is in bx:ax
    mov     dh, 0               ; want result in ds:si
    mov     dl, DevHlp_PhysToVirt
    call    _devhlp
    jc      phystovirterror
    mov     dx, ds
    mov     ax, si
    pop     es
    pop     si
    pop     ds
    ret
phystovirterror:
    pop     es
    pop     si
    pop     ds
    xor     dx, dx              ; return NULL pointer
    xor     ax, ax
    ret
PhysToVirt_      endp

;
; FARPOINTER    PhysToUVirt(FARPOINTER phys, USHORT length, USHORT mode);
; #pragma aux PhysToUVirt parm [ax bx] [cx] [dx]
;
PhysToUVirt_     proc    near    
    push    es
    xchg    ax, bx              ; arg is in bx:ax
    mov     dh, dl              ; mode
    mov     dl, DevHlp_PhysToUVirt
    call    _devhlp
    jc      phystouvirterror
    mov     dx, es
    mov     ax, bx
    pop     es
    ret
phystouvirterror:
    pop     es
    xor     dx, dx              ; return NULL pointer
    xor     ax, ax
    ret
PhysToUVirt_      endp

;
; FARPOINTER    AllocPhys(ULONG size, USHORT above1meg);
; #pragma aux AllocPhys parm [ax bx] [dx]
;
AllocPhys_     proc    near
    xchg    ax, bx              ; size is in bx:ax
    mov     dh, dl              ; mode
    mov     dl, DevHlp_AllocPhys
    call    _devhlp
    jc      allocphyserror
    mov     dx, ax
    mov     ax, bx
    ret
allocphyserror:
    xor     dx, dx              ; return NULL pointer
    xor     ax, ax
    ret
AllocPhys_      endp

;
; SHORT FreePhys(FARPOINTER phys);
; #pragma aux FreePhys parm [ax bx] 
;
FreePhys_     proc    near
    push    dx
    xchg    ax, bx              ; arg is in bx:ax
    mov     dl, DevHlp_FreePhys
    call    _devhlp
    jc      freephyserror
    pop     dx
    xor     ax, ax
    ret
freephyserror:
    pop     dx
    mov     ax, 1
    ret
FreePhys_      endp

 
 
 
;
; SHORT SendEvent(USHORT event, USHORT argument);
; #pragma aux SendEvent parm [ax bx]
;
SendEvent_     proc    near
    push    dx
    mov     ah,al                ; Event being signalled
    mov     dl,DevHlp_SendEvent
    call    _devhlp
    pop     dx
SendEvent_      endp 


;
; OFF    Limit(FARPOINTER address);
; #pragma aux Limit parm [ax bx]
;
Limit_      proc    near
    lsl     ax, bx
    ret
Limit_      endp

;
; void    InternalError(char * message, USHORT length);
; #pragma aux InternalError parm [si] [di]
;
InternalError_      proc    near
    mov     dl, DevHlp_InternalError
    call    _devhlp             ; point of no return
InternalError_      endp


;
; ULONG    VirtToLin(FARPOINTER virt)
; #pragma aux VirtToLin parm [ax bx]
;
VirtToLin_      proc    near

    xor     esi, esi
    mov     si, ax
    mov     ax, bx
    mov     dl, DevHlp_VirtToLin
    call    _devhlp
    jc      virttolinerror
    mov     dx, ax
    shr     eax, 16     
    xchg    ax, dx      ; result should now be in dx:ax
    ret
virttolinerror:
    xor     ax, ax
    xor     dx, dx
    ret
VirtToLin_      endp

;
; ULONG    VMAlloc(ULONG physAddress, ULONG far * result, ULONG length, ULONG flags);
; #pragma aux VMAlloc parm [di si] [es dx] [cx bx]
;
VMAlloc_      proc    near
    
    push    bp
    mov     bp, sp
    push    dx
    mov     ax, word ptr [bp+6]
    shl     eax, 16     ; shift left
    mov     ax, word ptr [bp+4]
    shl     ecx, 16     ; cx (count) already ok, shift it
    mov     cx, bx
    shl     edi, 16     ; di is already ok, shift it
    mov     di, si
    mov     dl, DevHlp_VMAlloc
    call    _devhlp
    jc      vmallocerror
    pop     bx          ; former dx now in bx
    mov     word ptr es:[bx], ax
    shr     eax, 16
    mov     word ptr es:[bx+2], ax
    pop     bp
    ret     4
vmallocerror:
    pop     dx
    mov     dx, ax
    shr     eax, 16     
    xchg    ax, dx      ; result should now be in dx:ax
    pop     bp
    ret     4
VMAlloc_      endp

;
; void    VMFree(ULONG physAddress);
; #pragma aux VMFree parm [dx ax]
;
VMFree_      proc    near
    
    xchg    ax, dx      ; higher bits to ax
    shl     eax, 16     ; shift left
    mov     ax, dx      ; lower bits to lower 16 bits of eax
    mov     dl, DevHlp_VMFree
    call    _devhlp
    ret
VMFree_      endp

_TEXT   ends
end
