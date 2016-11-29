.MODEL tiny
.CODE
;ORG 100h
ORG 0h

start: 		
			push ax
			push bx
			push cx
			push dx
			push es
			push ds ; saving data segment
			push si
			push di
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
init:	    call _init
;return to kernel
			pop di
			pop si
			pop ds
			pop es
			pop dx
			pop cx
			pop bx
			pop ax
			;retf ; return far to the kernel segment
			db 0cbh ; RETF code

extrn	_init:near
msg		db	'starting init...',13,10,0
END start