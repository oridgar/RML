//TEMP
#include <sysdef.h>
//END TEMP
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <io.h>

//FILE *fopen(const char *filename, const char *mode) {
//
//}

//int fclose(FILE *stream) {
//
//}

//int printf(const char *format, ...) {
//	va_list va;
//	int i = 0;
//	char *sval;
//	int ival;
//
//	//if (stream->fd == 0) {
//		va_start(va,format);
//		//(char *)va = format + sizeof(format);
//		while (format[i] != 0) {
//			if (format[i] != '%') {
//				putchar(format[i]);
//				i++;
//				continue;
//			}
//			i++;
//			switch (format[i]) {
//			case '\0':
//				break;
//			case 's':
//				//(((char*)((va) = ((va) + sizeof(char*)))));
//				//for (sval = va_arg(va,char*); *sval; sval++) {
//				for (sval = va_arg(va,char*); *sval; sval++) {
//					putchar(*sval);
//				}
//				//(char *)va = va + sizeof(char *);
//
//				i++;
//				break;
//			case 'd':
//				ival = va_arg(va,int);
//				_printstr(itoa(ival));
//				i++;
//				break;
//			}
//		}
//	//}
//	return i;
//}

int vfprintf(FILE *stream,const char *template,va_list ap) {
	int i = 0;
	char *sval;
	int ival;
	char strnum[7];

	if (stream->fd == 1) {
		while (template[i] != 0) {
			if (template[i] != '%') {
				//TODO: make sure converting parameter to int
				putchar(template[i]);
				i++;
				continue;
			}
			i++;
			switch (template[i]) {
			case '\0':
				break;
			case 's':
				for (sval = va_arg(ap,char*); *sval; sval++) {
					//TODO: make sure converting parameter to int
					putchar(*sval);
				}
				i++;
				break;
			case 'd':
				ival = va_arg(ap,int);
				//TODO: change it to write() function
				strcpy(strnum,itoa(ival));
				mywrite(1,strnum,strlen(strnum));
				//_printstr(itoa(ival));
				i++;
				break;
			}
		}
	}
	return i;
}

int fprintf(FILE *stream, const char *format, ...) {
	int retval;

	va_list va;
	va_start(va,format);
	retval = vfprintf(stream,format,va);
	va_end(va);
	return retval;
}

int printf(const char *format, ...) {
	FILE mystdout;
	int retval;

	va_list va;
	mystdout.fd = 1;
	va_start(va,format);
	retval = vfprintf(&mystdout,format,va);
	va_end(va);
	return retval;
}


//int fscanf(FILE *stream, const char *format, ...) {
//
//}

//int scanf(const char *format, ...) {
//	return 0;
//}

int fgetc(FILE *stream) {
	if (stream->fd == 0) {
		//TODO: change it to read() or fread()
		return (int)getchar();
	}
	return 0;
}

int fputc(int c, FILE *stream) {
	mywrite(stream->fd,(char *)(&c),1);
//	if (stream->fd == 1) {
//		//TODO: change it to write() or fwrite()
//		_putchar((char)c);
//	}
	return 0;

}

char getchar() {
	char in;
	//TODO: change it to fgetc() function
	//FILE mystdin;
	//mystdin.fd = 1;
	//return (char)fgetc(mystdin);
	myread(0,&in,1);
	return in;
	//return _getchar();
}

void putchar(char in) {
	//TODO: change it to fputc() function
	//FILE mystdout;
	//mystdout.fd = 1;
	//fputc((int)in,mystdout);
	mywrite(1,&in,1);
	//_putchar(in);
}


//size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream) {
//
//}

//size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream) {
//
//}
//int    fseek(FILE *stream, long offset, int origin) {
//
//}

