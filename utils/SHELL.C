#include "mystdio.h"
#include "mystring.h"
#include "linux\io.h"
#include "stdlib.h"

void main() {
	shell();
	//return 0;
}

int shell() {
	char *a = "# ";
	//char *buf = "                                                                                                               ";
	char buf[50];
	char in;
	int i;
	int j;
	int retcode;
	
	while (1) {
		//Prompt
		printstr("\r\n");
		printstr(a);
		i=0;
		//while character is not enter
		while ((in = getchar()) != 13) {
			//backspace but we are in the start of the prompt in the screen
			if (in == 8 && i == 0) {
				continue;
			}
			//backspace
			if (in == 8) {
				i--;
				mputchar(in);
				mputchar(' ');
				mputchar(in);
			}
			//write the character in the screen 
			else {
				mputchar(in);
				buf[i] = in;
				i++;
			}
			//ASCII 32 = whitespace
			//ASCII 08 = backspace
		}
		printstr("\r\n");
		buf[i] = '\0';
		if (strcmp(buf,"syscall") != -1) {
			call_int();
		}
		else if (strcmp(buf,"aaa") != -1) {
			_printstr("Hello kernel!");
		}
		else if (strcmp(buf,"bbb") != -1) {
			_putchar('S');
		}
		else if (strcmp(buf,"ccc") != -1) {
			for(j=0; j < 10; j++) {
				buf[j]=_getchar();
				_putchar(buf[j]);
			}
			buf[10]='\0';
			_printstr("\r\n");
			_printstr(buf);
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
		else if (strcmp(buf,"shell") != -1) {
			run_program("shell");
		}
		else if (strcmp(buf,"init") != -1) {
			run_program("init");
		}
		else if (strcmp(buf,"ls") != -1) {
			run_program("ls");
		}
		else if (strcmp(buf,"ps") != -1) {
			get_process_list();
		}
		else if (strcmp(buf,"demo") != -1) {
			retcode = run_program("demo");
			printstr("\r\nreturn code: ");
			mputchar('0' + retcode);
			printstr("\r\n");

		}
		else {
			//TODO: call to run_program and get return code from the kernel
			//by that to decide if to print the following message
			if (run_program(buf)) {
				printstr(buf);
				printstr(": command not found");
			}
		}
	}
	return 0;
}
