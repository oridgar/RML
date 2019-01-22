;org 0h

extern	main

_start: 		
			;mov [userss],cs
			;mov ss,[userss]
			;mov sp,0FFFEh
			
			;starting new stack frame
			push bp
			mov  bp,sp
			
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
;prt:		mov  si, offset msg
;			mov  ah, 0eh
;chr:		lodsb
;			or   al,al
;			;jz   ivt
;			jz   init
;			int  10h   ; call BIOS service to print next character
;			jmp  chr

;start init program
init:	    call main ;return to kernel
						
			pop di
			pop si
			pop ds
			pop es
			pop dx
			pop cx
			pop bx
			pop ax
			pop bp
			;retf ; return far to the kernel segment
			db 0cbh ; RETF code

;msg		db	'program entry point...',13,10,0
userss  dw  0
