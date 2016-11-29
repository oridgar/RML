#include "mystring.h"

int strcmp(char *src,char *dst) {
	//change return values and shell code associated with it!
	int i=0;
	int ret=-1;

	while (src[i] !=0 && dst[i] !=0 && src[i] == dst[i]) {
		i++;
	}
	if (src[i] == dst[i]) {
		ret=i;
	}
	return ret;
}


int  strncmp(char *src,char *dst,int n) {
	int i=0;
	
	while(src[i] !=0 && dst[i] !=0 && src[i] == dst[i] && i < n) {i++;}
	if (src[i] == dst[i]) {
		return 0;
	}
	//src is shorter than dst
	else if (src[i] == 0 && dst[i] != 0) {
		return -1;
	}
	else {
		return 1;
	}
}


size_t strlen(char *string) {
	size_t len =0;
	while(string[len] != 0) {len++;}
	return len;
}


void *memcpy(char *src, char *dst, int n) {
	int i;
	for(i=0; i < n ; i++) {
		dst[i] = src[i];
	}
	return dst;
}


void *memset(char *src, char c,int n) {
	int i;
	for(i=0; i < n; i++) {
		src[i] = c;
	}
	return src;
}
