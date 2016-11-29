#include "mystdio.h"
#include "stdlib.h"

void main() {
	char *str1 = "Entered to init...\r\n";
	char *str2 = "call to shell...\r\n";
	printstr(str1);
	printstr(str2);

	run_program("shell");
	//return 0;
}
