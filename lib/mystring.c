#include "mystring.h"

int strcmp(char *src,char *dst) {
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
