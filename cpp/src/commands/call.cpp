#include "neem.h"

int Neem::command_call(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	eof = index;
	for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
		if(instructions[index].type == label_ && instructions[index].value == parsed) return (int)index;
	}
	return alert('!', "Can't call '%s'", &index, &parsed);
}