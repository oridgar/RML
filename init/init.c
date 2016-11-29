#include "mystdio.h"

void main() {
	char *str1 = "Entered to init...\r\n";
	char *str2 = "call to shell...\r\n";
	/*
	int i = 0;
	while (str1[i] != 0) {
		mputchar(str1[i]);
		i++;
	} */
	printstr(str1);
	printstr(str2);
	run_shell();
	//return 0;
}