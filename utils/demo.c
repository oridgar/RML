#include <stdlib.h>
#include <stdio.h>

void main() {
	char string[13];
	int i;
	strcpy(string,"hello world!");
	printf(string);

	for(i=0; i < 5; i++) {
		string[i] = toupper(string[i]);
	}

	printf(string);
}
