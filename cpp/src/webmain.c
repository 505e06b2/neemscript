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

void webmain() {
	struct Neem* c = neem_new();
	//neem_interpretFile(c, argv[1]);
	neem_del(c);
	puts("Yo");
}

#ifdef __cplusplus
}
#endif  