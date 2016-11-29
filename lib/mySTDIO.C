#ifndef mystdio
#include "mystdio.h"
#define mystdio
#endif

//typedef unsigned long size_t;

void printstr(char *string) {
	
	/*int i = 0;
	while (string[i] != 0) {
		mputchar(string[i]);
		i++;
	}
	*/
	
	_printstr(string);
}

char getchar() {
	//char temp;
	
	//temp = _getchar();
	//return temp;
	return _getchar();
	
	/*waitForKey:
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
	*/
}

void mputchar(char in) {
	/*char temp = in;
	asm {
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
	}
	*/
	
	_putchar(in);
}

int printf(const char *format, ...) {
	int i = 0;
	while (format[i] != 0) {
		mputchar(format[i]);
		i++;
	}
	return i;
}

/*
int scanf(const char *format, ...) {
	return 0;
}


size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream) {
	
}

size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream) {
	
}
int    fseek(FILE *stream, long offset, int origin) {
	
}
*/