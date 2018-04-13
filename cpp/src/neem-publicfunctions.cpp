#include "neem.h"

Neem::~Neem() {
	cleanup();
}

Neem::Neem() { //Set up dynamic vars
	dynamicvariables["TIME"] = &Neem::dynamic_time;
	dynamicvariables["DATE"] = &Neem::dynamic_date;
	dynamicvariables["CD"] = &Neem::dynamic_cd;
	dynamicvariables["LS"] = &Neem::dynamic_ls;
	dynamicvariables["OS"] = &Neem::dynamic_os;
	dynamicvariables["UPPER"] = &Neem::dynamic_upper;
	dynamicvariables["LOWER"] = &Neem::dynamic_lower;
	dynamicvariables["PATH"] = &Neem::dynamic_path;
	dynamicvariables["EPOCH"] = &Neem::dynamic_epoch;
	dynamicvariables["STRFTIME"] = &Neem::dynamic_strftime;
	dynamicvariables["SYSTEM"] = &Neem::dynamic_system;
	dynamicvariables["POINTER"] = &Neem::dynamic_pointer;
}

void Neem::setVariable(const char *name, const char *value) {
	variables[name] = value;
}

const char *Neem::getVariable(const char *name) {
	std::map<const std::string, std::string>::iterator variableinter;
	if((variableinter = variables.find(name)) != variables.end()) return variableinter->second.c_str();
	return NULL;
}

//Interpreting
void Neem::interpretFile(const char *fname) {
	if(!readfilebyline(fname, &Neem::parseline)) return;
	runInstructions(); //The meat of the program
}

void Neem::interpretBuffer(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		uint32_t index = 0;
		for(; *buffer; buffer++) {
			if(*buffer == '\n') {
				currentline.push_back('\0');
				if(!parseline(currentline.data(), index++)) return;
				std::vector<char>().swap(currentline); //Clean the vector
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data(), index)) return;
	}
	
	runInstructions();
}

void Neem::interpretLine(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '&' && *(buffer+1) == '&') {
				currentline.push_back('\0');
				if(!parseline(currentline.data(), 0)) return;
				std::vector<char>().swap(currentline); //Clean the vector
				buffer++;
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data(), 0)) return;
	}
	
	runInstructions();
}