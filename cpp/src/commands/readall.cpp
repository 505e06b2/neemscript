#include "neem.h"

int Neem::command_readall(instruction *i, uint32_t index) {
	if(inputhandle == NULL) return alert('!', "No input file loaded; use the input command", &index);
	char buffer[MAX_LINE_LEN];
	std::string tempvar = "";
	while(fread(buffer, sizeof(char), sizeof(buffer), inputhandle)) tempvar += buffer;
	setvariabletoscope(parsevarval(&i->value), tempvar);
	return -1;
}