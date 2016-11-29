.MODEL tiny
.CODE
ORG 0h

global _farcall

buf_size EQU 288 ; in words, not bytes!

; INT 80h start points
start: 		
			;cli ; disable hardware interrupts while in system call
			jmp intr	;if jumps to address 0h - entry point for syscall
			jmp kernel  ;if jumps to address 3h - entry point for kernel.
			jmp sched
			

kernel: 	mov ax, cs ;initialize stack : SS=CS
			mov ss,ax
			mov ax, 0FFFEh
			mov sp,ax
			mov bp,ax
			mov ax, cs ; saving code segment
			mov ds,ax ; copying code segment to data segment
			jmp _startk
			
;prep_p:	;------------
			;SERVICE CODE
			;------------
			;mov  [service],dx ;copying the service code to local var
		
;start handling syscall
intr:		
			;ABOUT COPYING BUFFER FROM USER TO KERNEL
			;----------------------------------------
			
			;source:
			;at the beginning
			;DS is user space
			;SI is the address of param structure

			;target:
			;should do
			;ES to be the same as CS (kernel segment)
			;DI to be the address of a variable in kernel segment

			;ABOUT PARAMETERS
			;passing to kernel:
			;dx - service
			;si - params
			;MUST NOT OVERRIDE THEM UNTIL SAVING IN KERNEL SPACE OR USING THEM DIRECTLY!!!
			
			;DO NOT USING STACK AS THIS IS STILL THE USER STACK
			;all of these are saved in user stack
			;push bp ; saving previous stack frame
			;mov  bp,sp ; initializing new stack frame

			;DESTINATION OF STRING COPY
			;offset of data in the local segment
			mov  di,offset params ; destination of copy is params global variable in kernel
			
			;----------------------
			;SYSTEM CALL PARAMETERS
			;----------------------			
			;copy destination of buffer is the current segment
			;local data segment temporary as ES
			mov  ax,cs ;copying CS to ES in order to copy paramer structure
			mov	 es,ax
			
			;X.COPY BUFFER FROM USER SPACE TO KERNEL SPACE
			;-------------
			;copy function
			;-------------
			mov cx,buf_size
			cld ; direction is forward
cpy:		lodsw ; copy next word from user space to AX : LODSW (DS:SI -> AX) and SI++
			stosw ; copy next word from AX to kernel space : STOSW (AX -> ES:DI) and DI++
			loop cpy
			;---------------
			;end of function
			;---------------
			
			; CHANGE TO KERNEL SPACE
			;switch data segment
			mov ax,ds ;temporary saves user data segment
			;-------------------------------------------------
			;Align data segment to be the same as code segment
			;-------------------------------------------------
			;DO NOT USING STACK AS THIS IS STILL THE USER STACK
			;push cs ; saving code segment
			;pop  ds ; copying code segment to data segment
			mov bx,cs
			mov ds,bx ; data segment is on kernel
			mov [dsuser],ax ;saved the user space data segment
			

			;switch stack
			;increase 2 bytes (each address offset is 2 bytes)
			inc [suserp]
			inc [suserp]
			mov bx,[suserp] ;backup stack pointer
			mov [ssuser+bx], ss
			;END OF USER STACK
			mov ss,[ssbak]
			;switch stack pointer
			mov [spuser+bx], sp
			mov sp,[spbak]
			
			;saving data segment and buffer to return value
			mov  ax,si
			sub  ax,buf_size*2 ;REWIND user buffer
			push ax ;to use user parameters offset to return value
			push [dsuser] ;to use user data segment to return value

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
			mov cx,buf_size
			cld ; direction is forward
cpy2:		lodsw ; copy next word from kernel space to AX
			stosw ; copy next word from AX to user space
			loop cpy2
			;---------------
			;end of function
			;---------------			

			jmp  sysexit

sysexit:	;HERE SHOULD BE THE CODE TO BRING BACK USER SPACE!
			
			;//////////////////////
			;CONTEXT SWITCH TO USER
			;//////////////////////
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

sched:
			push ax
			push bx
			push ds
			mov ax,cs
			mov ds,ax
			mov bx, offset _b_int8_offs
			mov ax, [bx]
			mov jmpf_ofs, ax
			mov bx, offset _b_int8_seg
			mov ax,[bx]
			mov jmpf_segm, ax

			;user code
			;mov ax,61
			;push ax
			;call __putchar
			;pop ax
			call _schedule

			pop ds
			pop bx
			pop ax
			;far jmp to bios int 8
			;public _b_int8_seg
			;public _b_int8_offs

			db 0EAh ; JUMP FAR instruction
			jmpf_ofs  dw 0
    		jmpf_segm dw 0h



			
;the caller first pushes the offset and then the segment!
;stack content: offset, segment, caller offset to the next instruction, caller base pointer
			
;_farcall PROC
_farcall:
	;ARG in_ofs:WORD
	;ARG in_segm:WORD
	
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
	mov ofs, ax
	
	mov ax, word ptr [bp+2] ;first parameter
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

public _b_int8_seg
public _b_int8_offs

;msg		db	13,10,'system call!',13,10,0
params		db buf_size*2 DUP(0)
service		dw 0
spbak		dw 0
ssbak		dw 0
spuser		dw 10 DUP(0)
ssuser		dw 10 DUP(0)
suserp      dw 0
dsuser		dw 0
_b_int8_seg	dw ?
_b_int8_offs	dw ?

extrn	__getchar:near
extrn	__putchar:near
extrn	__dispatch:near
extrn	_halt:near
extrn	_reboot:near
extrn	_startk:near
extrn   _schedule:near

END start
