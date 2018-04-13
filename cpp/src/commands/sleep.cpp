#include "neem.h"

int Neem::command_sleep(instruction *i, uint32_t index) {
	threadsleep(stoul(parsevarval(&i->value)));
	return -1;
}