#include "neem.h"

int Neem::command_unloadlib(instruction *i, uint32_t index) {
	std::string parsed = parsevarval(&i->value).c_str();
	auto it = loadedlibs.find(parsed);
	if(it == loadedlibs.end()) return alert('#', "Library not loaded: '%s'", &index, &parsed);
	freelibrary(it->second);
	loadedlibs.erase(it); //Remove from map
	return -1;
}