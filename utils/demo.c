#include <stdlib.h>
#include <stdio.h>

void main() {
	char string[13];
	int i;
	unsigned long a = 1048576;

	printf("%s\r\n",ultoa(a));

	strcpy(string,"hello world!");
	printf(string);

	for(i=0; i < 5; i++) {
		string[i] = toupper(string[i]);
	}

	printf(string);
}
