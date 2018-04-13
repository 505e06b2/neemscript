#include "neem.h"

int Neem::command_cd(instruction *i, uint32_t index) {
	if(i->extravalue != "") fprintf(outputhandle, "%s\n", getcurrentdir().c_str()); //Print if no args
	else if(chdir(parsevarval(&i->value).c_str()) != 0) {
		std::string parsed = parsevarval(&i->value);
		return alert('!', "Can't cd to '%s'", &index, &parsed);
	}
	return -1;
}