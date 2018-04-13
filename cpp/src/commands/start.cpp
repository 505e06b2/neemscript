#include "neem.h"

int Neem::command_start(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	FILE *f = popen(parsed.c_str(), "r");
	FILE *stderrbackup = stderr;
	if(f == NULL) return alert('!', "Could not open program: '%s'", &index, &parsed);
				
	char buffer[MAX_LINE_LEN];
	while(fgets(buffer, MAX_LINE_LEN, f) != NULL) fprintf(outputhandle, "%s", buffer);
	return -1;
}