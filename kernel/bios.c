#include <bios.h>

int read_sectors(unsigned int in_segment,
		         int in_buf_offset,
		         char in_num_sectors,
				 char in_cylinder,
				 char in_sector,
				 char in_head,
				 char in_drive) {

	short ax;
	short bx;
	short cx;
	short dx;
	short es;

	ax = in_num_sectors + 0x02 << 8;
	bx = in_buf_offset;
	cx = in_sector + in_cylinder << 8;
	dx = in_drive + in_head << 8;
	es = in_segment;

	char myret;

	__asm__ (
			"mov  es,%0\n\t"  //second 64K segment (pass by value)
			:
			: "r" (es)
	);

	__asm__ (
		"push ax\n\t"
		"push bx\n\t"
		"push cx\n\t"
		"push dx\n\t"
		"push es\n\t"
		//"mov  dl,[drive]\n\t" //drive
		//"mov  ch,[cylinder]\n\t" // cylinder/track
		//"mov  dh,[head]\n\t" //head 0
		//"mov  cl,[sector]\n\t"
		//"mov  al,[num_sectors]\n\t" //number of sectors to read
		//"mov  bx,[segment]\n\t"
		//"mov  es,bx\n\t"  //second 64K segment (pass by value)
		//"mov  bx,[buf_offset]\n\t" //load to buf_offset in the segment
		//"mov  ah,0x02\n\t" //Read Sectors From Drive service
		"int  0x13\n\t"
		"mov  %0,ah\n\t"
		"pop es\n\t"
		"pop dx\n\t"
		"pop cx\n\t"
		"pop bx\n\t"
		"pop ax\n\t"
		: "=ah" (myret)
		: "a" (ax), "b" (bx), "c" (cx), "d" (dx)//, "es" (es)
	);
	return (int)myret;
}

//==============
//BIOS FUNCTIONS
//==============

char b_getchar() {
	char temp;
	__asm__ (
		"waitForKey:\n\t"
		"mov ah,0x01\n\t"
		"int 0x16\n\t"
		"jnz gotKey\n\t"
		"jmp waitForKey\n\t"
		"gotKey:\n\t"
		"mov ah,0x0\n\t"
		"int 0x16\n\t"
		"mov %0,al\n\t"
		: "=r" (temp)
	);

	return temp;
}

char b_getchar_sync() {
	char temp;
	__asm__ (
		"mov ah,0x0\n\t"
		"int 0x16\n\t"
		"mov %0,al\n\t"
		: "=r" (temp)
	);
	return temp;
}

unsigned char b_query_kb_stat() {
	char result;
	__asm__ (
		"mov ah,0x1\n\t"
		"int 0x16\n\t"
		"jnz gotKey2\n\t"
		"mov %0, 0\n"
		"gotKey2:\n\t"
		"mov %0, 1\n\t"
		: "=r" (result)
	);
	return result;
}

void reboot() {
	__asm__ (
		"int 0x19\n\t"
	);
}

void read_RTC_time(unsigned int *out_hours,unsigned int *out_minutes,unsigned int *out_seconds) {
	unsigned char hours, minutes, seconds;
	__asm__ (
		"push ax\n\t"
		"push cx\n\t"
		"push dx\n\t"
		"mov ah,0x02\n\t"
		"int 0x1A\n\t"
		"mov %0,ch\n\t"
		"mov %1,cl\n\t"
		"mov %2,dh\n\t"
		"pop dx\n\t"
		"pop cx\n\t"
		"pop ax\n\t"
		: "=r" (hours), "=r" (minutes), "=r" (seconds)
	);
	*out_hours = ((hours & 0xF0) >> 4) * 10 + (hours & 0x0F);
	*out_minutes = ((minutes & 0xF0) >> 4) * 10 + (minutes & 0x0F);
	*out_seconds = ((seconds & 0xF0) >> 4) * 10 + (seconds & 0x0F);
}

void read_RTC_date(unsigned int *out_year,unsigned int *out_month,unsigned int *out_day) {
	unsigned char century,year,month,day;
	__asm__ (
		"push ax\n\t"
		"push cx\n\t"
		"push dx\n\t"
		"mov ah,0x04\n\t"
		"int 0x1A\n\t"
		"mov %0,ch\n\t"
		"mov %1,cl\n\t"
		"mov %2,dh\n\t"
		"mov %3,dl\n\t"
		"pop dx\n\t"
		"pop cx\n\t"
		"pop ax\n\t"
		: "=r" (century), "=r" (year), "=r" (month), "=r" (day)
	);
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
	__asm__ (
		  "push ax\n\t"
		  "mov ah,0x0E\n\t"
		  "mov al,%0\n\t"
		  "int 0x10\n\t"
		  "pop ax\n\t"
		  :
		  : "r" (temp)
	);
}
