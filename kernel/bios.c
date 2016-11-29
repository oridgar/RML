#include "bios.h"

int read_sectors(unsigned int segment,int buf_offset, char num_sectors,char cylinder,char sector, char head, char drive) {
	char myret;

	asm {
		push ax
		push bx
		push cx
		push dx
		push es
		mov  dl,[drive] //drive
		mov	 ch,[cylinder] // cylinder/track
		mov  dh,[head] //head 0
		mov  cl,[sector]
		mov  al,[num_sectors] //number of sectors to read
		mov  bx,[segment]
		mov  es,bx  //second 64K segment (pass by value)
		mov  bx,[buf_offset] //load to 0h (binary file org 0 instead of com file) TODO: fix taking from offset param
		mov  ah,0x02 //Read Sectors From Drive service
		int  13h
		mov  [myret],ah
		pop es
		pop dx
		pop cx
		pop bx
		pop ax
	}
	return (int)myret;
}

/*
void reboot() {
	asm {
		int 19h
	}
}

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

*/

void b_putchar(char in) {
	char temp = in;
	asm {
		  push ax
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
		  pop ax
	}
}
