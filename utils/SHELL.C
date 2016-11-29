#include "mystdio.h"
#include "mystring.h"
#include "linux\io.h"

int shell() {
	char *a = "# ";
	char *buf = "                                                                                                               ";
	char in;
	int i;
	
	sti();
	while (1) {
		printstr("\r\n");
		printstr(a);
		i=0;
		while ((in = getchar()) != 13) {
			if (in == 8 && i == 0) {
				continue;
			}
			if (in == 8) {
				i--;
				mputchar(in);
				mputchar(' ');
				mputchar(in);
			}
			else {
				mputchar(in);
				buf[i] = in;
				i++;				
			}
			//ASCII 32 = whitespace
			//ASCII 08 = backspace
			//need to check if the key is backspace, need to decrease the buffer and clean the character from the screen
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
		else if (strcmp(buf,"exit") != -1) {
			break;
		}
		else if (strcmp(buf,"dispatch") != -1) {
			dispatch();
		}
		else {
			printstr(buf);
			printstr(": command not found");
		}
	}
	return 0;
}