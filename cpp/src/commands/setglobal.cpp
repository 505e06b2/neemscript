#include "neem.h"

int Neem::command_setglobal(instruction *i, uint32_t index) {
	variablesinscopes[0][parsevarval(&i->value)] = parsevarval(&i->extravalue);
	return -1;
}