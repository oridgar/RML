#include <string.h>
#include <types.h>

char *strcpy(char *dst, char *src) {
	int i;
	i = -1;
	do {
		i++;
		dst[i] = src[i];
	} while (src[i] != 0);
	return dst;
}
char *strncpy(char *dst,char *src,unsigned int n) {
	int i;
	i = -1;
	do {
		i++;
		dst[i] = src[i];
	} while (src[i] != 0 && i < n);

	i++;
	while (i < n) {
		dst[i] = 0;
	}
	return dst;
}

int strcmp(char *src,char *dst) {
	//change return values and shell code associated with it!
	//int i=0;
	//int ret=-1;

	for ( ; *src == *dst; src++, dst++)
		if (*src == '\0')
			return 0;
	return ((*(unsigned char *)src < *(unsigned char *)dst) ? -1 : 1);

//	while (src[i] !=0 && dst[i] !=0 && src[i] == dst[i]) {
//		i++;
//	}
//	if (src[i] == dst[i]) {
//		ret=0;
//	}
//	else if (src[i] == 0) {
//		ret = -1;
//	}
//	else if (dst[i] == 0) {
//		ret = 1;
//	}
//	return ret;
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


void *memcpy(char *dst, char *src, int n) {
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
