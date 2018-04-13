#include "neem.h"

int Neem::command_pause(instruction *i, uint32_t index) {
	printf("Press ENTER to continue...");
	getchar();
	return -1;
}