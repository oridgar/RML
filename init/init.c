#include <stdlib.h>
#include <stdio.h>

void main() {
	char *str1 = "Entered to init...\r\n";
	char *str2 = "call to shell...\r\n";
	printf(str1);
	printf(str2);

	run_program("shell");
	//return 0;
}
