#include "neemWrapper.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {  
#endif

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("[!] No file specified\n");
		return 1;
	}
	
	struct Neem* c = neem_new();
	neem_interpretFile(c, argv[1]);
	printf(">test: %s\n", neem_getVariable(c, "test"));
	//neem_interpretBuffer(c, "echo line one\necho line 2\nset gg=geea \necho %gg%");
	neem_del(c);
	
	return 0;
}

#ifdef __cplusplus
}
#endif  