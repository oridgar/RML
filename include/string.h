#ifndef _STRING_H
#define _STRING_H
#include <types.h>


char *strcpy(char *dst, char *src);
char *strncpy(char *dst,char *src,unsigned int n);
int  strcmp(char *src,char *dst);
int  strncmp(char *src,char *dst,int n);
size_t strlen(char *string);
void *memcpy(char *dst, char *src, int n);
void *memset(char *src, char c,int n);

#endif
