#include "neem.h"

std::string Neem::parsevariables(const char *value) { //the index is a return index
	char buffer[PARSE_BUFFER_LEN];
	char varnamebuffer[MAX_VARNAME_LEN] = {0};
	uint8_t varnameindex;
	bool writevarname = false;
	
	uint16_t outindex = 0; //Needs to be outside the for scope for the \0
	
	for(uint16_t i = 0; value[i]; i++) {
		if(value[i] == '%') {
			if(writevarname) { //becoming false
				std::map<const std::string, std::string>::iterator variableinter;
				std::map<const std::string, std::function<std::string()>>::iterator globalvariableinter;
				strtok(varnamebuffer, ":"); //for the substring
				if( (globalvariableinter = globalvariables.find(varnamebuffer)) != globalvariables.end() || //Global first
						(variableinter = variables.find(varnamebuffer)) != variables.end() ) { //Variable exists, so we get it from the map
					std::string tempvar = (globalvariableinter != globalvariables.end()) ? globalvariableinter->second() : variableinter->second; //It is a global
					uint8_t variablelen = tempvar.length();
					const char *variable = tempvar.c_str();
					
					//Substring
					int16_t start = 0;
					uint8_t len = -1; //max
					
					char *substring;
					if(substring = strtok(NULL, ",")) { //This substring is the start index
						start = atoi(substring+1);
						if(start < 0) start = variablelen + start;
						if(start < 0) start = 0; //make sure start is 0
						if(substring = strtok(NULL, "")) { //End index
							int16_t l = atoi(substring);
							len = (l < 0) ? variablelen + l : start + l;
						}
					}
					
					for(uint8_t i = 0 + start; (i < variablelen) && (i < len); i++) {
						buffer[outindex++] = variable[i]; //could use strcpy but I want the output index to roll over; will continue until \0
					}
				}
			} else {
				varnameindex = 0; //reset index
				memset(varnamebuffer, 0, sizeof(varnamebuffer)); // set all to 0 so we don't need to \0 at the end
			}
			writevarname = !writevarname;
		} else if(writevarname) {
			varnamebuffer[varnameindex++] = value[i];
		} else {
			buffer[outindex++] = value[i];
		}
	}
	buffer[outindex] = '\0';
	return buffer;
}
