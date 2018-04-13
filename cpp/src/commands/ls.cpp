#include "neem.h"

int Neem::command_ls(instruction *i, uint32_t index) {
	fprintf(outputhandle, "%s\n", listdir(parsevarval(&i->value).c_str(), '\n').c_str());
	return -1;
}