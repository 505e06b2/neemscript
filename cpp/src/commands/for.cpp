#include "neem.h"

int Neem::command_for(instruction *i, uint32_t index) {
	parsedstrings parsed;
	parseallstrings(&parsed, i);
	std::string array = getvariablefromscope(parsed.extravalue);
	const char *therest = array.c_str();
	std::string currentvalue = "";
	
	for(; *therest; therest++) {
		if(strncmp(therest, parsed.xxxtravalue.c_str(), parsed.xxxtravalue.size()) == 0) {
			therest += parsed.xxxtravalue.size();
			break;
		} else {
			currentvalue += *therest;
		}
	}
				
	if(currentvalue == "") { //Loop done
		int ret = searchfortag(&index, rof_, for_);
		if(ret >= 0) return ret;
		return alert('!', "No matching 'rof' for For loop", &index);
	}
				
	setvariabletoscope(parsed.extravalue, therest);
	setvariabletoscope(parsed.value, currentvalue);
	return -1;
}