#ifndef stddef
#include "stddef.h"
#define stddef
#endif

int  strcmp(char *src,char *dst);
int  strncmp(char *src,char *dst,int n);
size_t strlen(char *string);
void *memcpy(char *src, char *dst, int n);
void *memset(char *src, char c,int n);

