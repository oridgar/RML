.MODEL tiny
.CODE
ORG 0h

; INT 80h start points
start: 		;cli
			jmp intr	;if jumps to address 0h - entry point for syscall
			
			;if jumps to address 3h - entry point for kernel.
kernel: 	mov ax, 3000h ;initialize stack
			mov ss,ax
			mov ax, 0FFFEh
			mov sp,ax
			mov bp,ax
			mov ax, cs ; saving code segment
			mov ds,ax ; copying code segment to data segment
			jmp _startk

intr:	 	push ax
			push bx
			push cx
			push dx
			push es
			push ds ; saving data segment
			;copy
			push cs ; saving code segment
			pop  ds ; copying code segment to data segment

; this will be replaced with the C function, mapping the register values as parameters
disp:		push ax ; parameter for dispatch function
			call __dispatch
			pop  ax ; remove the parameter
			jmp  sysexit

; this also should be converted to C			
prt:		mov  ah, 0eh
			mov  si, offset msg
chr:		lodsb
			or   al,al
			jz   sysexit
			int  10h   ; call BIOS service to print next character
			jmp  chr

; this also should be converted to C			
halt:		hlt		

; this also should be converted to C			
reboot:		int 19h	

;dosint:		mov bx, 0
;			mov dx, offset msg
;			mov ah, 9
;			int 21h
sysexit:	pop ds
			pop es
			pop dx
			pop cx
			pop bx
			pop ax
			;sti
			iret
			
msg		db	13,10,'system call!',13,10,0
extrn	__getchar:near
extrn	__putchar:near
extrn	__dispatch:near
extrn	_halt:near
extrn	_reboot:near
extrn	_startk:near
END start