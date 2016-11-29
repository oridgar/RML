.MODEL tiny
.CODE
ORG 100h

start: 		;cli
			push ds ; saving data segment
			push cs ; saving code segment
			pop  ds ; copying code segment to data segment

disp:		cmp al,00h ; print something
			je  prt
			cmp al,01h ; halt the computer
			je  halt
			cmp al,02h ; reboot
			je reboot
			
prt:		mov  ah, 0eh
			mov  si, offset msg
chr:		lodsb
			or   al,al
			jz   sysexit
			int  10h   ; call BIOS service to print next character
			jmp  chr

halt:		hlt		

reboot:		int 19h	

;dosint:		mov bx, 0
;			mov dx, offset msg
;			mov ah, 9
;			int 21h
sysexit:	pop ds
			;sti
			iret

msg		db	13,10,'system call!',13,10,0
END start