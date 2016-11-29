void xload(char *name);
void xrun(char *name);
void reboot();
void halt();
void uname();
void set_ivt();
void clear_scr();
void run_init();

char _getchar() {
	char temp;
	waitForKey:
	asm {
		mov ah,01h
		int 16h
		jnz gotKey
		jmp waitForKey
	}

	gotKey:
	asm {
		mov ah,00h
		int 16h
		mov [temp],al
	}
	return temp;
}

void _putchar(char in) {
	char temp = in;
	asm {
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
	}
}

void _dispatch(char param) {
	switch (param) {
		case 0:
			_putchar('a');
			_putchar('b');
			_putchar('c');
			break;
		case 1:
			halt();
			break;
		case 2:
			reboot();
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			break;
	}
	/*
			;cmp al,00h ; print something
			;je  prt
			cmp al,01h ; halt the computer
			;je  halt
			call _halt
			cmp al,02h ; reboot
			;je reboot
			call _reboot
			;cmp al,03h ; getchar
			;call __getchar
			;cmp al,04h ; putchar
			;call __putchar
			cmp al,05h ; testing
	*/
	return;
}

void startk() {
	//xload("shell");
	//xrun("shell");
	//_dispatch();
	set_ivt();
	while (1) { run_init(); }
	reboot();
}

void xload(char *name) {
	int shseg = 2000;
	//loading shell
	loadsh:
	asm {
		    mov  es,[shseg] 
			mov  ah,0x02 
			mov  al,0x02 
			mov	 ch,0x00 
			inc  cl      
			mov  dh,0x00 
			mov  dl,0x00 
			mov  bx,0x100 
			int  13h 
	}
}

void xrun(char *name) {
	//asm {
		//call  SHSEG:0100h
	//}
}

void reboot() {
	asm {
		int 19h
	}
}

void halt() {
	asm {
		hlt
	}
}

void uname() {
	return;
}

void set_ivt() {
	asm {
			mov ax,0h
			mov es,ax
						
			mov bx,200h
			mov ax,0h
			mov es:[bx],ax
			
			mov bx,202h
			mov ax,1000h
			mov es:[bx],ax
	}
	/*
	ivt:		; first segment set at ES
			mov ax,0h
			mov es,ax ; ivt is in first segment
						
			; position for INT 80h in IVT - offset of ISR
			mov bx,200h ; int 80h pos
			;mov ax,100h ; address of ISR
			mov ax,0h ; address of ISR
			mov es:[bx],ax ; location of the code for interrupt
			
			; position for INT 80h in IVT - segment of ISR
			mov bx,202h
			mov ax,1000h
			mov es:[bx],ax ; segment for interrupt 80h
	*/
}

void run_init() {
	asm {
		db 9Ah // CALL FAR instruction
		//db  0eah //JMP FAR instruction 
		dw  0 //offset
		dw  2000h //segment
	}
	//macro example
	/*
	FARCALL macro seg, ofs 
        db 9Ah  ; CALL FAR instruction 
        dw ofs 
        dw seg 
	endm 
        And then use 
	FARCALL 0043h, 0130h 
	*/
	return;
}