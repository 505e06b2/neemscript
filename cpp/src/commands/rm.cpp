#include "neem.h"

int Neem::command_rm(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(remove(parsed.c_str()) != 0) return alert('!', "Could not remove '%s'; if it's a directory, use rmdir", &index, &parsed);
	return -1;
}