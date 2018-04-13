#include "neem.h"

int Neem::command_rmdir(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(!removedir(parsed.c_str())) return alert('!', "Error while removing directory '%s'", &index, &parsed);
	return -1;
}