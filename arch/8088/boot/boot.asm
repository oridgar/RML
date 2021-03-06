; boot loader
; this will only load the whole boot loader (1024 bytes)
; and 

org	0x7c00

SHSEG EQU 2000h

start:
			cli
			mov cx,2
	
nextprt:	loop prt
			je   loadblk
prt:		mov  si, msg
			mov  ah, 0x0e
loop:		lodsb
			or   al,al
			jz   nextprt
			int  0x10   ; call BIOS service to print next character
			jmp  loop

;load the rest of the boot loader
loadblk:    mov  ah,0x02 ; Read Sectors From Drive service
			mov  al,0x01 ; number of sectors
			mov	 ch,0x00 ; cylinder (always 0 for floppy)
			mov  cl,0x02 ; start from SECTOR 2 (sector 1 is the MBR already loaded by BIOS)
			mov  dh,0x00 ; head
			mov  dl,0x00 ; drive
			mov  bx,loadsh ; load sector 2 to address of part2
			int  13h ; call BIOS tio read sectors from floppy
			jmp  loadsh

;data
msg:		db	`booting real mode linux...\r\n`, 0
		    times  510 -($-$$) db 0
			dw	   0xaa55

;from here starts the second sector

;part2:
			;mov cx,10
;nextprt2:	loop prt2
			;je loadsh ; if loop has finished writing all characters, go to halt
;prt2:		mov si, msg2
			;mov ah, 0x0e
;loop2:		lodsb
			;or  al,al
			;jz  nextprt2
			;int 0x10
			;jmp loop2
;			
;halt:		hlt

;floppy geometry 1.44M
;cylinders 80
;sectors per cylinder 18
;heads 1

			
; loading init + shell to memory
loadsh:     jmp loddsp
;			mov  es,[shseg] ; second 64K segment
;			mov  ah,0x02 ; Read Sectors From Drive service
;			mov  al,0x02 ; number of sectors to read
;			mov	 ch,0x00 ; cylinder/track 
;			inc  cl      ; start from SECTOR 3
;			mov  dh,0x00 ; head
;			mov  dl,0x00 ; drive
			;mov  bx,0x100 ; load to 100h as com files relocated to this address
;			mov  bx,0x0 ; load to 100h as com files relocated to this address
			;int  13h ; call BIOS to read sectors from floppy
			;add  bx,0x2200 ; 17x512=8704 in decimal and 2200 in hex
			;mov  al,0x12 ; from now on number of sectors to read will be 18 as in 1.44MB disk

;lodnext:	inc  ch      ; move to the next cylinder
;			;cmp  ch,0x51 ; end of tracks. 50h is 80 in decimal
;			cmp  ch,0x06 ; 6 tracks is 64512 bytes, enough to fill the segment
;			je   loddsp    ; finished loading dispatcher for int 80h
;			mov  cl,1    ; new track, first sector
;			int  13h ; call BIOS to read sectors from floppy
;			add  bx,0x2400 ; 18x512=9216 in decimal and 2400 in hex
;			jmp  lodnext

;loading kernel to memory
loddsp:		mov  ah,0x02 ; Read Sectors From Drive service
			mov  al,0x10 ; number of sectors
			mov  cl,0x03 ; starting from sector
			;mov  bx,0x100 ;  COM entry point
			mov  bx,0x0 ;  entry point
			mov  es,[kernseg] ; third 64K segment
			int  13h ; call BIOS to read sectors from floppy
loddsp2:	mov  ah,0x02 ; Read Sectors From Drive service
			mov  al,0x12 ; number of sectors
			mov  cl,0x01 ; starting from sector
			mov  dh,0x01 ; head
			mov  bx,0x2000 ;  entry point
			mov  es,[kernseg] ; third 64K segment
			int  13h ; call BIOS to read sectors from floppy
loddsp3:	mov  ah,0x02 ; Read Sectors From Drive service
			mov  al,0x12 ; number of sectors
			mov  cl,0x01 ; starting from sector
			mov  dh,0x00 ; head
			mov  ch,0x01 ; track
			mov  bx,0x4400 ;  entry point
			mov  es,[kernseg] ; third 64K segment
			int  13h ; call BIOS to read sectors from floppy

			
;start kernel			
;init:		jmp  SHSEG:0100h
;init:		jmp  SHSEG:0000h
init:		jmp  0x1000:0x0002 ;entry point for kernel

;data section

;msg2:		db  `error!`, 0
shseg:		dw  SHSEG			
kernseg:	dw  0x1000			
			times 1024 - ($-$$) db 0
			
