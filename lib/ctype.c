#include <ctype.h>

int tolower(int c) {
	if('A' <= c && c <= 'Z') {
		return ('a' + c - 'A');
	}
	else {
		return c;
	}
}
int toupper(int c) {

	if('a' <= c && c <= 'z') {
		return ('A' + c - 'a');
	}
	else {
		return c;
	}
}
