#include "neem.h"

int Neem::command_loadlib(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(!loadlibrary(parsed.c_str(), parsed.size())) return alert('!', "Could not load library: '%s'", &index, &parsed);
	return -1;
}