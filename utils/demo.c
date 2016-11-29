#include "mystdio.h"
#include "stdlib.h"

void main() {
	char string[13];
	int i;
	strcpy(string,"hello world!");
	printstr(string);

	for(i=0; i < 5; i++) {
		string[i] = toupper(string[i]);
	}

	printstr(string);
}
