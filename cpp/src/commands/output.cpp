#include "neem.h"

int Neem::command_output(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(outputhandle != stdout) fclose(outputhandle);
	if(parsed == "reset") outputhandle = stdout;
		else outputhandle = fopen(parsed.c_str(), parsevarval(&i->extravalue).c_str());
	if(outputhandle == NULL) return alert('!', "Could not open file: '%s'", &index, &parsed);
	return -1;
}