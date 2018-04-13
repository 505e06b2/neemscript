#include "neem.h"

int Neem::command_else(instruction *i, uint32_t index) {
	int ret = searchfortag(&index, fi_, if_);
	if(ret >= 0) return ret;
	return alert('!', "No matching 'fi' for else", &index);
}