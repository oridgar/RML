#include "ctype.h"

int tolower(int c) {
	return ('a' + c - 'A');
}
int toupper(int c) {
	return ('A' + c - 'a');
}
