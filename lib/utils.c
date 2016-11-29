#include <utils.h>

int stolower(char *string) {
	int i;
	for(i=0; i < strlen(string); i++) {
		string[i] = tolower(string[i]);
	}
	return 0;
}
int stoupper(char *string) {
	int i;
	for(i=0; i < strlen(string); i++) {
		string[i] = toupper(string[i]);
	}
	return 0;
}
