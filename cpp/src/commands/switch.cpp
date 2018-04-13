#include "neem.h"

int Neem::command_switch(instruction *i, uint32_t index) {
	switchcheckstring = parsevarval(&i->value);
	return -1;
}