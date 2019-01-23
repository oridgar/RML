#include <linux/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int shell() {
	char *a = "# ";
	char buf[50];
	char in;
	int i;
	int j;
	int retcode;
	
	while (1) {
		//Prompt
		printf("\r\n");
		printf(a);
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
				putchar(in);
				putchar(' ');
				putchar(in);
			}
			//write the character in the screen 
			else {
				putchar(in);
				buf[i] = in;
				i++;
			}
			//ASCII 32 = whitespace
			//ASCII 08 = backspace
		}
		printf("\r\n");
		buf[i] = '\0';
		if (strcmp(buf,"syscall") == 0) {
			call_int();
		}
		else if (strcmp(buf,"aaa") == 0) {
			//_printstr("Hello kernel!");
			printf("Hello kernel!");
		}
		else if (strcmp(buf,"bbb") == 0) {
			//_putchar('S');
			putchar('S');
		}
		else if (strcmp(buf,"ccc") == 0) {
			for(j=0; j < 10; j++) {
				buf[j]=getchar();
				//_putchar(buf[j]);
				putchar(buf[j]);
			}
			buf[10]='\0';
//			_printstr("\r\n");
//			_printstr(buf);
			printf("\r\n");
			printf(buf);
		}
		else if (strcmp(buf,"halt") == 0) {
			halt();
		}
		else if (strcmp(buf,"reboot") == 0) {
			reboot();
		}
		else if (strcmp(buf,"exit") == 0) {
			break;
		}
		else if (strcmp(buf,"date") == 0) {
			time();
		}
		else {
			//TODO: change run_program to system()
			if (system(buf)) {
				printf(buf);
				printf(": command not found");
			}
		}
	}
	return 0;
}

void main() {
	shell();
	//return 0;
}
