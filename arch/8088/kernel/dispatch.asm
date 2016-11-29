.MODEL tiny
.CODE
ORG 0h

global _farcall

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
			push si
			push di
			push si ; to use user parameters offset to return value
			push ds ; to use user data segment to return value
			
prep_p:		;------------
			;SERVICE CODE
			;------------
			mov  [service],dx ;copying the service code to local var

			;----------------------
			;SYSTEM CALL PARAMETERS
			;----------------------
			;local data segment temporary as ES
			mov  ax,cs ;copying CS to ES in order to copy paramer structure
			mov  es,ax
			
			;offset of data in the local segment
			mov  di,offset params ; destination of copy is params global variable in kernel

			;-------------
			;copy function
			;-------------
			mov cx,256 ;TODO: make sure the size will be right
			cld ; direction is forward
cpy:		lodsw ; copy next word from user space to AX
			stosw ; copy next word from AX to kernel space
			loop cpy
			;---------------
			;end of function
			;---------------
			
			;-------------------------------------------------
			;Align data segment to be the same as code segment
			;-------------------------------------------------
			push cs ; saving code segment
			pop  ds ; copying code segment to data segment

;calling to C function
disp:		mov  bx,offset params
			push bx
			push dx ; parameter for dispatch function
			call __dispatch
			pop dx ; remove the parameter
			pop bx ; bx is dummy
			
			;---------------------------------
			;Prepare return parameters to user
			;---------------------------------
			;ds is already point to kernel space
			pop es ; user space data segment to ES
			pop di
			mov si, offset params
			
			;-------------
			;copy function
			;-------------
			mov cx,256 ;TODO: make sure the size will be right
			cld ; direction is forward
cpy2:		lodsw ; copy next word from kernel space to AX
			stosw ; copy next word from AX to user space
			loop cpy2
			;---------------
			;end of function
			;---------------			
						
			jmp  sysexit

; this also should be converted to C			
;prt:		mov  ah, 0eh
;			mov  si, offset msg
;chr:		lodsb
;			or   al,al
;			jz   sysexit
;			int  10h   ; call BIOS service to print next character
;			jmp  chr

; this also should be converted to C			
;halt:		hlt		

; this also should be converted to C			
;reboot:		int 19h	

;dosint:		mov bx, 0
;			mov dx, offset msg
;			mov ah, 9
;			int 21h
sysexit:	pop di
			pop si
			pop ds
			pop es
			pop dx
			pop cx
			pop bx
			pop ax
			;sti
			iret

;_farcall PROC
_farcall:
	;ARG in_ofs:WORD
	;ARG in_segm:WORD
	
	;the caller first pushes the offset and then the segment!
	
	push bp
	mov  bp,sp
	add  bp,2
	mov ax, word ptr [bp+2] ;first parameter
	;mov ax, 2000h
	mov segm, ax
	mov ax, word ptr [bp+4]
	;mov ax,0000h
	mov ofs, ax
    db 9Ah  ; CALL FAR instruction 
    ofs  dw 0
    segm dw 0h
	pop bp
	RET
;_farcall ENDP


			
;msg		db	13,10,'system call!',13,10,0
params		db 512 DUP(0)
service		dw 0
extrn	__getchar:near
extrn	__putchar:near
extrn	__dispatch:near
extrn	_halt:near
extrn	_reboot:near
extrn	_startk:near
END start