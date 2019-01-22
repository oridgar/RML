#include <utils.h>
#include <string.h>
#include <ctype.h>

int stolower(char *string) {
	int i;
	for(i=0; i < strlen(string); i++) {
		string[i] = (char)tolower((int)string[i]);
	}
	return 0;
}
int stoupper(char *string) {
	int i;
	for(i=0; i < strlen(string); i++) {
		string[i] = (char)toupper((int)string[i]);
	}
	return 0;
}
