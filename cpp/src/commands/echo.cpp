#include "neem.h"

int Neem::command_echo(instruction *i, uint32_t index) {
	fprintf(outputhandle, "%s\n", parsevarval(&i->value).c_str());
	return -1; //-1 is the 0 of this function; anything positive becomes the new line index
}