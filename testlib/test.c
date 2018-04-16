#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETURN "TESTFUNC: RETURN"

char *testfunc(char *arg) {
	if(arg) printf("TESTFUNC: %s\n", arg);
	char *buffer = malloc(sizeof(RETURN) * sizeof(char));
	if(buffer != NULL) {
		memcpy(buffer, RETURN, sizeof(RETURN));
		return buffer;
	}
	return NULL;
}