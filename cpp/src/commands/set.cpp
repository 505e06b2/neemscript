#include "neem.h"

int Neem::command_set(instruction *i, uint32_t index) {
	variables[parsevarval(&i->value)] = parsevarval(&i->extravalue);
	return -1;
}