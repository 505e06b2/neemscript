#include "neem.h"

int Neem::command_goto(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(parsed == "eof") {
		uint32_t tempeof = eof;
		eof = -2; //-2 or it overflows
		return (int)tempeof;
	}
	for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
		if(instructions[index].type == label_ && instructions[index].value == parsed) return (int)index;
	}
	return alert('!', "Can't goto '%s'", &index, &parsed);
}