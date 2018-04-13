#include "neem.h"

int Neem::command_setsystem(instruction *i, uint32_t index) {				
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	if(!setenvvar(&parsed.value, &parsed.extravalue)) {
		alert('!', "Can't put '%s' into an environment variable", &index, &parsed.extravalue);
		return -2;
	}
	return -1;
}