#include "neem.h"

int Neem::command_case(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value);
	if(parsed != "" && switchcheckstring != parsed) {
		int ret = searchfortag(&index, case_, none_);
		if(ret >= 0) return ret-1;
		return alert('!', "No default case for switch", &index);
	}
	return -1;
}