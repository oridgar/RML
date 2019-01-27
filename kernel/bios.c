#include <bios.h>


int read_sectors(unsigned int segment,int buf_offset, char num_sectors,char cylinder,char sector, char head, char drive) {
	char myret;
	short ax;
	short bx;
	short cx;
	short dx;
	short es;

	ax = num_sectors + 0x02 << 8;
	bx = buf_offset;
	cx = sector + cylinder << 8;
	dx = drive + head << 8;
	es = segment;

	__asm__ __volatile__ (
	  "  push %%ax\n"
	  "  push %%bx\n"
	  "  push %%cx\n"
	  "  push %%dx\n"
	  "  push %%es\n"
	  "  mov  %1, %%dl\n"   //drive
	  "  mov  %2, %%ch\n"   // cylinder/track
	  "  mov  %3, %%dh\n"   //head 0
	  "  mov  %4, %%cl\n"
	  "  mov  %5, %%al\n"   //number of sectors to read
	  "  mov  %6, %%bx\n"
	  "  mov  %%bx, %%es\n"   //second 64K segment (pass by value)
	  "  mov  %7, %%bx\n"   //load to buf_offset in the segment
	  "  mov  $0x2, %%ah\n" //Read Sectors From Drive service
	  "  int  $0x13\n"
	  "  mov  %%ah, %0\n"
	  "  pop  %%es\n"
	  "  pop  %%dx\n"
	  "  pop  %%cx\n"
	  "  pop  %%bx\n"
	  "  pop  %%ax\n"
	  :"=m"(myret)
	  :"m"(drive), "m"(cylinder), "m"(head), "m"(sector), "m"(num_sectors), "m"(segment), "m"(buf_offset)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
	return (int)myret;
}

//==============
//BIOS FUNCTIONS
//==============

char b_getchar() {
	char temp;

	__asm__ __volatile__ (
	  "waitForKey:\n"
	  "  mov  $0x1, %%ah\n"
	  "  int  $0x16\n"
	  "  jnz  gotKey\n"
	  "  jmp  waitForKey\n"
	  "gotKey:\n"
	  "  mov  $0x0, %%ah\n"
	  "  int  $0x16\n"
	  "  mov  %%al, %0\n"
	  :"=m"(temp)
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);

	return temp;
}

char b_getchar_sync() {
	char temp;
	__asm__ __volatile__ (
	  "  mov  $0x0, %%ah\n"
	  "  int  $0x16\n"
	  "  mov  %%al, %0\n"
	  :"=m"(temp)
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
	return temp;
}

unsigned char b_query_kb_stat() {
	char result;
	__asm__ __volatile__ (
		"  mov $0x1, %%ah\n"
		"  int $0x16\n"
		"  jnz gotKey2\n"
		"  movb $0, %0\n"
		"gotKey2:\n"
		"  movb $1, %0\n"
		: "=m" (result)
		:
		:"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
	return result;
}

void reboot() {
	__asm__ (
		"int $0x19\n"
	);
}

void read_RTC_time(unsigned int *out_hours,unsigned int *out_minutes,unsigned int *out_seconds) {
	unsigned char hours, minutes, seconds;
	__asm__ __volatile__ (
	  "  push %%ax\n"
	  "  push %%cx\n"
	  "  push %%dx\n"
	  "  mov  $0x2, %%ah\n"
	  "  int  $0x1a\n"
	  "  mov  %%ch, %0\n"
	  "  mov  %%cl, %1\n"
	  "  mov  %%dh, %2\n"
	  "  pop  %%dx\n"
	  "  pop  %%cx\n"
	  "  pop  %%ax\n"
	  :"=m"(hours), "=m"(minutes), "=m"(seconds)
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
	*out_hours = ((hours & 0xF0) >> 4) * 10 + (hours & 0x0F);
	*out_minutes = ((minutes & 0xF0) >> 4) * 10 + (minutes & 0x0F);
	*out_seconds = ((seconds & 0xF0) >> 4) * 10 + (seconds & 0x0F);
}

void read_RTC_date(unsigned int *out_year,unsigned int *out_month,unsigned int *out_day) {
	unsigned char century,year,month,day;
	__asm__ __volatile__ (
	  "  push %%ax\n"
	  "  push %%cx\n"
	  "  push %%dx\n"
	  "  mov  $0x4, %%ah\n"
	  "  int  $0x1a\n"
	  "  mov  %%ch, %0\n"
	  "  mov  %%cl, %1\n"
	  "  mov  %%dh, %2\n"
	  "  mov  %%dl, %3\n"
	  "  pop  %%dx\n"
	  "  pop  %%cx\n"
	  "  pop  %%ax\n"
	  :"=m"(century), "=m"(year), "=m"(month), "=m"(day)
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
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
	__asm__ __volatile__ (
	  "  push %%ax\n"
	  "  mov  $0xe, %%ah\n"
	  "  mov  %0, %%al\n"
	  "  int  $0x10\n"
	  "  pop  %%ax\n"
	  :
	  :"m"(temp)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}
