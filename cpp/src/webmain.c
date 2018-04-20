#include "neemWrapper.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {  
#endif

/*
int main() {
	puts("Loaded!");
	return 0;
}
*/

void webmain(const char *input) {
	struct Neem* c = neem_new();
	neem_interpretFile(c, input);
	neem_del(c);
}

#ifdef __cplusplus
}
#endif  