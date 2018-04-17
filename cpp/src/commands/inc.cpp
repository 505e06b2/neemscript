#include "neem.h"

int Neem::command_inc(instruction *i, uint32_t index) {
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	std::string varvalue = getvariablefromscope(parsed.value);
	int increaseby = 1;
	if(varvalue != "") {
		if(i->extravalue != "") {
			if(parsed.extravalue != "") increaseby = stoi(parsed.extravalue);
				else {
					increaseby = 0;
					alert('#', "'%s' isn't defined", &index, &parsed.value);
				}
		}
	} else varvalue = "-1";
	setvariabletoscope(parsed.value, std::to_string(stoll(varvalue) + increaseby));
	return -1;
}