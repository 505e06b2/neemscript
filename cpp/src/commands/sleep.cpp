#include "neem.h"

int Neem::command_sleep(instruction *i, uint32_t index) {
	std::this_thread::sleep_for(std::chrono::milliseconds( stoi(parsevarval(&i->value)) ));
	return -1;
}