#include "neem.h"

int Neem::command_setlocal(instruction *i, uint32_t index) {
	std::map<const std::string, std::string> firstscope;
	variablesinscopes.push_back(firstscope);
	return -1;
}