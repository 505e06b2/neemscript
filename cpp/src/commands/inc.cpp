#include "neem.h"

int Neem::command_inc(instruction *i, uint32_t index) {
	std::map<const std::string, std::string>::iterator variableinter;
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	if((variableinter = variables.find(parsed.value)) != variables.end()) { //Variable exists, so we get it from the map
		int increaseby = 1;
		if(i->extravalue != "") {
			if(parsed.extravalue != "") increaseby = stoi(parsed.extravalue);
				else {
					increaseby = 0;
					alert('#', "'%s' isn't defined", &index, &parsed.value);
				}
		}
		variables[parsed.value] = std::to_string(stoi(variables[parsed.value]) + increaseby);
		return -1;
	}
	return alert('!', "Can't inc '%s'", &index, &parsed.value);
}