#include <stdlib.h>


char *itoa(int i) {
	static char  buf[5 + 2];
	char *p = buf + 5 + 1;
	buf[6]='\0';
	if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
		return p;
	}
	else {
		do {
			*--p = '0' - (i % 10);
			i /= 10;
		} while (i != 0);
		*--p = '-';
	}
	return p;
}
