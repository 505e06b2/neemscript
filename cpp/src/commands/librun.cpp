#include "neem.h"

int Neem::command_librun(instruction *i, uint32_t index) {
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	if(loadedlibs.find(parsed.value) == loadedlibs.end()) return alert('!', "Library not loaded: '%s'", &index, &parsed.value);
	char *(*functionpointer)(const char *) = getlibraryfunction(&parsed.value, parsed.extravalue.c_str());
	if(functionpointer == NULL) return alert('!', "Cannot run function '%s'", &index, &parsed.extravalue);
	free(functionpointer(parsed.xxxtravalue.c_str())); //free just in case
	return -1;
}