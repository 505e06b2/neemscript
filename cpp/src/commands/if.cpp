#include "neem.h"

int Neem::command_if(instruction *i, uint32_t index) {
	if(!i->check( parsevarval(&i->value), parsevarval(&i->extravalue) )) {
		int ret = searchfortag(&index, else_, if_);
		if(ret >= 0) return ret;
		ret = searchfortag(&index, fi_, if_);
		if(ret >= 0) return ret;
		return alert('!', "No matching 'fi' for if", &index);
	}
	return -1;
}