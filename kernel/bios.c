#include <bios.h>

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
		mov  bx,[buf_offset] //load to buf_offset in the segment
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

//==============
//BIOS FUNCTIONS
//==============

char b_getchar() {
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

char b_getchar_sync() {
	char temp;
	asm {
		mov ah,00h
		int 16h
		mov [temp],al
	}
	return temp;
}

unsigned char b_query_kb_stat() {
	asm {
		mov ah,01h
		int 16h
		jnz gotKey
	}
	return 0;

	gotKey:
	return 1;
}

void reboot() {
	asm {
		int 19h
	}
}

void read_RTC_time(unsigned int *out_hours,unsigned int *out_minutes,unsigned int *out_seconds) {
	unsigned char hours, minutes, seconds;
	asm {
		push ax
		push cx
		push dx
		mov ah,02h
		int 1Ah
		mov [hours],ch
		mov [minutes],cl
		mov [seconds],dh
		pop dx
		pop cx
		pop ax
	}
	*out_hours = ((hours & 0xF0) >> 4) * 10 + (hours & 0x0F);
	*out_minutes = ((minutes & 0xF0) >> 4) * 10 + (minutes & 0x0F);
	*out_seconds = ((seconds & 0xF0) >> 4) * 10 + (seconds & 0x0F);
}

void read_RTC_date(unsigned int *out_year,unsigned int *out_month,unsigned int *out_day) {
	unsigned char century,year,month,day;
	asm {
		push ax
		push cx
		push dx
		mov ah,04h
		int 1Ah
		mov [century],ch
		mov [year],cl
		mov [month],dh
		mov [day],dl
		pop dx
		pop cx
		pop ax
	}
	*out_year = ((century & 0xF0) >> 4) * 1000 + (century & 0x0F) * 100
		+ ((year & 0xF0) >> 4) * 10 + (year & 0x0F);
	*out_month = ((month & 0xF0) >> 4) * 10 + (month & 0x0F);
	*out_day = ((day & 0xF0) >> 4) * 10 + (day & 0x0F);
}



//==================
//END BIOS FUNCTIONS
//==================

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
