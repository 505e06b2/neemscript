#include "neem.h"

int Neem::command_runlibfunc(instruction *i, uint32_t index) {
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	if(loadedlibs.find(parsed.value) == loadedlibs.end()) return alert('!', "Library not loaded: '%s'", &index, &parsed.value);
	int ret = runlibraryfunction(&parsed.value, parsed.extravalue.c_str(), parsed.xxxtravalue.c_str());
	if(ret == -27202) return alert('!', "Could not load '%s' in: %s", &index, &parsed.extravalue, &parsed.value);
		else if(ret != 0) return alert('#', "Error in function: '%s'", &index, &parsed.extravalue);
	return -1;
}