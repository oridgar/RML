.MODEL tiny
.CODE
ORG 0h

global _farcall

; INT 80h start points
start: 		
			cli ; disable hardware interrupts while in system call
			jmp intr	;if jumps to address 0h - entry point for syscall
			
			;if jumps to address 3h - entry point for kernel.
kernel: 	;mov ax, 3000h ;initialize stack
			mov ax, cs ;initialize stack
			mov ss,ax
			mov ax, 0FFFEh
			mov sp,ax
			mov bp,ax
			mov ax, cs ; saving code segment
			mov ds,ax ; copying code segment to data segment
			jmp _startk

;start handling syscall
;intr:	 	;all of these are saved in user stack
			;push bp ; saving previous stack frame
			;mov  bp,sp ; initializing new stack frame
			
			; saving all relevant registers
;			pushf  ; [bp-2]
;			push ax ;[bp-4]
;			push bx ; [bp-6]
;			push cx ; [bp-8]
;			push dx ; [bp-10]
;			push es ; [bp-12]
;			push ds ; [bp-14]
;			push si ; [bp-16]
;			push di ; [bp-18]
			
			;END OF USER STACK
			
			
			;mov es,ax 
			
			; CHANGE TO KERNEL SPACE

			
			;END OF CONTEXT SWITCH
			
			;push si ; [bp-20] to use user parameters offset to return value
			;push ds ; [bp-22] to use user data segment to return value
			
;prep_p:		;------------
			;SERVICE CODE
			;------------
			;mov  [service],dx ;copying the service code to local var

			;----------------------
			;SYSTEM CALL PARAMETERS
			;----------------------
			;local data segment temporary as ES
			;mov  ax,cs ;copying CS to ES in order to copy paramer structure
			;mov  es,ax
			
			;DESTINATION OF STRING COPY
			;offset of data in the local segment
intr:		mov  di,offset params ; destination of copy is params global variable in kernel
			
			;copy destination of buffer is the current segment
			mov  ax,cs
			mov	 es,ax
			
			;-------------
			;copy function
			;-------------
			mov cx,254
			cld ; direction is forward
cpy:		lodsw ; copy next word from user space to AX
			stosw ; copy next word from AX to kernel space
			loop cpy
			;---------------
			;end of function
			;---------------
			
			;switch data segment
			mov ax,ds ;temporary saves user data segment
			mov bx,cs
			mov ds,bx ; data segment is on kernel
			mov [dsuser],ax ;saved the user space data segment
			
			;switch stack
			;increase 2 bytes (each address offset is 2 bytes)
			inc [suserp]
			inc [suserp]
			mov bx,[suserp] ;backup stack pointer
			mov [ssuser+bx], ss
			mov ss,[ssbak]
			;switch stack pointer
			mov [spuser+bx], sp
			mov sp,[spbak]
			
			
			;saving data segment and buffer to return value
			mov  ax,si
			sub  ax,254*2
			push ax
			push [dsuser]
			
			;-------------------------------------------------
			;Align data segment to be the same as code segment
			;-------------------------------------------------
			;push cs ; saving code segment
			;pop  ds ; copying code segment to data segment

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
			mov cx,254
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
sysexit:	;HERE SHOULD BE THE CODE TO BRING BACK USER SPACE!

;			pop di
;			pop si
;			pop ds
;			pop es
;			pop dx
;			pop cx
;			pop bx
;			pop ax
;			popf
;			pop bp
			
			;CONTEXT SWITCH TO USER
			;kernel stack backup
			mov [ssbak],ss
			mov [spbak],sp
			;user stack
			mov bx,[suserp] ;backup stacks - stack pointer
			mov ss,[ssuser+bx]
			mov sp,[spuser+bx]
			;decrease 2 bytes (each address offset is 2 bytes)
			dec [suserp]
			dec [suserp]
			;user data
			mov ax,es
			mov ds,ax
			
			sti ; enable hardware interrupts
			iret

;_farcall PROC
_farcall:
	;ARG in_ofs:WORD
	;ARG in_segm:WORD
	
	;the caller first pushes the offset and then the segment!
	;stack content: offset, segment, caller offset to the next instruction, caller base pointer
	
	;new stack frame
	push bp
	mov  bp,sp
	add  bp,2
	
	;==================================
	;CONTEXT SWITCH FROM KERNEL TO USER
	;==================================
	
	;saving registers in the kernel stack
	pushf  ; [bp-2]
	push ax ;[bp-4]
	push bx ; [bp-6]
	push cx ; [bp-8]
	push dx ; [bp-10]
	push es ; [bp-12]
	push ds ; [bp-14]
	push si ; [bp-16]
	push di ; [bp-18]
	
	;backing up stack pointer & segment
	mov [spbak],sp
	mov [ssbak],ss
	
	;=====
	;LOGIC
	;=====
	
	;offset for jumping
	mov ax, word ptr [bp+4]
	;mov ax,0000h
	mov ofs, ax
	
	mov ax, word ptr [bp+2] ;first parameter
	;mov ax, 2000h
	mov segm, ax ; segment for jump far

	mov ss,ax ; stack segment is the same as code segment for user app
	mov sp, 0FFFEh ; user's top stack segment 
				
    db 9Ah  ; CALL FAR instruction 
    ofs  dw 0
    segm dw 0h
	
	;==================================
	;CONTEXT SWITCH FROM USER TO KERNEL
	;==================================
	
	;switch to user data segment
	;restoring data segment
	mov ax,cs
	mov ds,ax
	
	;restoring stack segment & pointer
	mov sp,[spbak]
	mov ss,[ssbak]
	
	pop di
	pop si
	pop ds
	pop es
	pop dx
	pop cx
	pop bx
	pop ax
	popf
	
	pop bp
	RET
;_farcall ENDP


			
;msg		db	13,10,'system call!',13,10,0
params		db 512 DUP(0)
service		dw 0
spbak		dw 0
ssbak		dw 0
spuser		dw 10 DUP(0)
ssuser		dw 10 DUP(0)
suserp      dw 0
dsuser		dw 0


extrn	__getchar:near
extrn	__putchar:near
extrn	__dispatch:near
extrn	_halt:near
extrn	_reboot:near
extrn	_startk:near
END start