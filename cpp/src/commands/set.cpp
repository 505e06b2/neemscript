#include "neem.h"

int Neem::command_set(instruction *i, uint32_t index) {
	setvariabletoscope(parsevarval(&i->value), parsevarval(&i->extravalue));
	return -1;
}