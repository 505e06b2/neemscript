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
	
	//testing pointers
	const char testarray[] = "did it work??";
	char pointerbuffer[(sizeof(long long)*2) +1 +2]; //2 values per byte +1 for \0 +2 for 0x
	sprintf(pointerbuffer, "0x%llx", (unsigned long long)testarray);
	neem_setVariable(c, "pointer", pointerbuffer);
	
	//Testing features
	//neem_setVariable(c, "test", "thetestvalue");
	//neem_interpretLine(c, "echo gg&& echo wow");
	//neem_interpretBuffer(c, "echo line one\necho line 2\nset gg=geea \necho %gg%");
	neem_interpretFile(c, argv[1]);
	//printf(">test: %s\n", neem_getVariable(c, "test"));
	
	neem_del(c);
	
	return 0;
}

#ifdef __cplusplus
}
#endif  