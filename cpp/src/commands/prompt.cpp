#include "neem.h"

int Neem::command_prompt(instruction *i, uint32_t index) {
	char buff[128];
	fprintf(outputhandle, "%s", parsevarval(&i->extravalue).c_str());
	if(fgets(buff, sizeof(buff), stdin) == NULL) return alert('!', "Something is wrong with stdin?", &index);
	buff[strlen(buff)-1] = '\0'; //remove \n
	variables[parsevarval(&i->value)] = buff;
	return -1;
}