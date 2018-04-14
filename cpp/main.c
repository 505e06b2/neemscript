#include "neemWrapper.h"
#include <stdio.h>

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("[!] No file specified\n");
		return 1;
	}
	
	struct Neem* c = neem_new();
	neem_interpretFile(c, argv[1]);
	neem_del(c);
	
	return 0;
}