#include "mystdio.h"
#include "mystring.h"
#include "linux\io.h"

int shell() {
	char *a = "# ";
	char *buf = "                                                                                                               ";
	char in;
	int i;

	do {
		sti();
		printstr("\r\n");
		printstr(a);
		i=0;
		while ((in = getchar()) != 13) {
			mputchar(in);
			buf[i] = in;
			i++;
		}
		printstr("\r\n");
		buf[i] = '\0';
		if (strcmp(buf,"syscall") != -1) {
			call_int();
		}
		else if (strcmp(buf,"halt") != -1) {
			halt();
		}
		else if (strcmp(buf,"reboot") != -1) {
			reboot();
		}
		else {
			printstr(buf);
			printstr(": command not found");
		}
	} while (strcmp(buf,"exit") == -1);
	return 0;
}