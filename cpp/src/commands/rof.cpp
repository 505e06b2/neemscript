#include "neem.h"

int Neem::command_rof(instruction *i, uint32_t index) {
	uint8_t forloops = 1;
	for(int32_t a = index; a >= 0; a--) {
		if(instructions[a].type == for_) {
			if(--forloops == 0) return (a-1); //-1 or it will go to the line after
		} else if(instructions[a].type == for_) {
			forloops++;
		}
	}
	return alert('!', "No matching 'for' for rof", &index);
}