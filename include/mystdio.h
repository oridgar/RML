#ifndef stddef
#include "stddef.h"
#define stddef
#endif
//my simple functions
char getchar();
void mputchar(char in);
void printstr(char *string);


typedef struct 
{
 short level ;
 short token ;
 short bsize ;
 char fd ;
 unsigned flags ;
 unsigned char hold ;
 unsigned char *buffer ;
 unsigned char * curp ;
 unsigned istemp; 
}FILE ;

//file operations
//---------------
FILE *fopen(const char *filename, const char *mode);
FILE *freopen(const char *filename, const char *mode,FILE *stream);
int fflush(FILE *stream);
int fclose(FILE *stream);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);

FILE *tmpfile(void);
//char *tmpnam(char s[L_tmpnam]);
//int setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);

//formatted output
int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *s, const char *format, ...);

//formatted input
int fscanf(FILE *stream, const char *format, ...);
int scanf(const char *format, ...);
sscanf(char *s, const char *format, ...);

//character input and output functions
int fgetc(FILE *stream);
char *fgets(char *s, int n, FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int getc(FILE *stream);
//int getchar(void);
char *gets(char *s);
int putc(int c, FILE *stream);
int putchar(const char *s);
int ungetc(int c, FILE *stream);

//size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream);
//size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream);
int    fseek(FILE *stream, long offset, int origin);