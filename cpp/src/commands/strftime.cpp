#include "neem.h"

int Neem::command_strftime(instruction *i, uint32_t index) {
	variables[parsevarval(&i->value)] = getstrftime(256, parsevarval(&i->extravalue).c_str());
	return -1;
}