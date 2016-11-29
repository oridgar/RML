.MODEL tiny
.CODE
ORG 100h

start: 		
			mov ax, 3000h ;initialize stack
			mov ss,ax
			mov ax, 0ffffh
			mov sp,ax
			mov ax, cs ; saving code segment
			mov ds,ax ; copying code segment to data segment
prt:		mov  si, offset msg
			mov  ah, 0eh
chr:		lodsb
			or   al,al
			jz   ivt
			int  10h   ; call BIOS service to print next character
			jmp  chr

;dosint:		mov bx, 0
;			mov dx, offset msg
;			mov ah, 9
;			int 21h

ivt:		; first segment set
			mov ax,0h
			mov es,ax ; ivt is in first segment
						
			mov bx,200h ; int 80h
			mov ax,100h
			mov es:[bx],ax ; location of the code for interrupt
			
			mov bx,202h
			mov ax,1000h
			mov es:[bx],ax ; segment for interrupt 80h

init:	    sti
			call _init
	    	int 19h

extrn	_init:near
msg		db	'loading kernel...',13,10,0
END start