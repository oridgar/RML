.MODEL tiny
.CODE
;ORG 100h
ORG 0h

start: 		
			push ds ; saving data segment
			mov ax, cs ; saving code segment
			mov ds,ax ; copying code segment to data segment

;printing message			
prt:		mov  si, offset msg
			mov  ah, 0eh
chr:		lodsb
			or   al,al
			;jz   ivt
			jz   init
			int  10h   ; call BIOS service to print next character
			jmp  chr

;start init program
init:	    sti
			call _init
;return to kernel
			pop ds
			retf ; return far to the kernel segment

extrn	_init:near
msg		db	'loading init...',13,10,0
END start