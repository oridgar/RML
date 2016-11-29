#include "mystdio.h"

void printstr(char *string) {
	int i = 0;
	while (string[i] != 0) {
		mputchar(string[i]);
		i++;
	}

}

char getchar() {
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

void mputchar(char in) {
	char temp = in;
	asm {
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
	}
}
