#include "neem.h"

int Neem::command_input(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(parsed == "reset") inputhandle = NULL;
		else inputhandle = fopen(parsed.c_str(), "r");
	if(inputhandle == NULL) return alert('!', "Could not open file: '%s'", &index, &parsed);
	return -1;
}