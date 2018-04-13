#include "neem.h"

Neem::~Neem() {
	cleanup();
}

Neem::Neem() { //Set up dynamic vars
	dynamicvariables["TIME"] = dynamic_time;
	dynamicvariables["DATE"] = dynamic_date;
	dynamicvariables["CD"] = dynamic_cd;
	dynamicvariables["LS"] = dynamic_ls;
	dynamicvariables["OS"] = dynamic_os;
	dynamicvariables["UPPER"] = dynamic_upper;
	dynamicvariables["LOWER"] = dynamic_lower;
	dynamicvariables["PATH"] = dynamic_path;
	dynamicvariables["EPOCH"] = dynamic_epoch;
	dynamicvariables["STRFTIME"] = dynamic_strftime;
	dynamicvariables["SYSTEM"] = dynamic_system;
	dynamicvariables["POINTER"] = dynamic_pointer;
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
	if(!readfilebyline(fname, parseline)) return;
	runInstructions(); //The meat of the program
}

void Neem::interpretBuffer(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '\n') {
				currentline.push_back('\0');
				if(!parseline(currentline.data())) return;
				std::vector<char>().swap(currentline); //Clean the vector
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data())) return;
	}
	
	runInstructions();
}

void Neem::interpretLine(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '&' && *(buffer+1) == '&') {
				currentline.push_back('\0');
				if(!parseline(currentline.data())) return;
				std::vector<char>().swap(currentline); //Clean the vector
				buffer++;
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data())) return;
	}
	
	runInstructions();
}