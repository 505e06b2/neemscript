#include "neem.h"

int Neem::command_endlocal(instruction *i, uint32_t index) {
	if(variablesinscopes.size() == 1) return alert('!', "Endlocal without startlocal? You're about to remove the highest scope...", &index);
	variablesinscopes.pop_back();
	return -1;
}