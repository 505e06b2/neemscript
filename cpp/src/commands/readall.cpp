#include "neem.h"

int Neem::command_readall(instruction *i, uint32_t index) {
	if(inputhandle == NULL) return alert('!', "No input file loaded; use the input command", &index);
	char buffer[MAX_LINE_LEN];
	std::string parsed = parsevarval(&i->value);
	variables[parsed] = "";
	while(fread(buffer, sizeof(char), sizeof(buffer), inputhandle)) variables[parsed] += buffer;
	return -1;
}