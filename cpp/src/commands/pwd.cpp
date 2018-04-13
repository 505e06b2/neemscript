#include "neem.h"

int Neem::command_pwd(instruction *i, uint32_t index) {
	fprintf(outputhandle, "%s\n", getcurrentdir().c_str());
	return -1;
}