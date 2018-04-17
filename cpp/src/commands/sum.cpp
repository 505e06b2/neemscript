#include "neem.h"

int Neem::command_sum(instruction *i, uint32_t index) {
	setvariabletoscope(parsevarval(&i->value), std::to_string(dosum(parsevarval(&i->extravalue).c_str())));
	return -1;
}