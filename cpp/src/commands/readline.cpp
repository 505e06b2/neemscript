#include "neem.h"

int Neem::command_readline(instruction *i, uint32_t index) {
	if(inputhandle == NULL) return alert('!', "No input file loaded; use the input command", &index);
		char buffer[MAX_LINE_LEN];
		std::string parsed = parsevarval(&i->value);
		if(fgets(buffer, sizeof(buffer), inputhandle) == NULL) {
			setvariabletoscope(parsed, "");
		} else {
			if(i->value == "") return -1; //Put the line into a variable
			uint16_t len = strlen(buffer);
			for(int index = len; index > len-3 && index >= 0; index--) {
				if(buffer[index] == '\r' || buffer[index] == '\n') {
					buffer[index] = '\0';
				}
			}
			setvariabletoscope(parsed, (buffer[0] == '\0') ? " " : buffer);
		}
	return -1;
}