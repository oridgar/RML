.model tiny

.code
global N_LXURSH@

; Long Unsigned Divide
;N_LUDIV@ PROC
;	ARG number1:DWORD
;	ARG number2:DWORD
;	mov ax, [number1 + 2]
;	mov dx, [number1]
;	mov
;	RET
;N_LUDIV@ ENDP


; Long Right Shift
N_LXURSH@ PROC
next:	mov ch,0
		shr ax,1
		shr dx,1 ;rcr dx, 1
		jnc cond
		or  ax,8000h
cond:	loop next
		RET
N_LXURSH@ ENDP

; Long Modulus
;N_LUMOD@ PROC
;	ARG number1:DWORD
;	ARG number2:DWORD

;	RET
;N_LUMOD@ ENDP

end
