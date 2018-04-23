#include "neem.h"

int Neem::command_echor(instruction *i, uint32_t index) {
	fprintf(outputhandle, "%s", parsevarval(&i->value).c_str());
	return -1;
}