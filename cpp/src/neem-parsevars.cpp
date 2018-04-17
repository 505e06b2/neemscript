#include "neem.h"

std::string Neem::parsevariables(const char *value, const char searchfor, uint8_t *amount) { //the index is a return index
	std::vector<char> buffer;
	std::vector<char> varnamebuffer;
	bool writevarname = false;
	
	for(; *value; value++) {
		if(*value == searchfor) {
			if(!writevarname && *(value+1) == searchfor) {
				buffer.push_back(searchfor);
				value++;
				continue;
			}
			if(writevarname) { //becoming false
				
				varnamebuffer.push_back('\0');
				size_t varnamelen = varnamebuffer.size();
				char *varname = varnamebuffer.data();
				char *startindex = NULL;
				
				for(char *temp = varname; *temp; temp++) {
					if(*temp == ':' && *(temp+1) == '~') {
						*temp = '\0';
						startindex = temp+2;
						break;
					}
				}
				
				std::string varvalue;
				{ //Check
					char *possibleparams = splitstring(varname, ';');
					
					std::map<const std::string, std::string (Neem::*)(char *)>::iterator gvar;
					
					if((gvar = dynamicvariables.find(varname)) != dynamicvariables.end()) {
						varvalue = (this->*gvar->second)(possibleparams);
					} else {
						varvalue = getvariablefromscope(varname);
					}
				}
				
				if(varvalue != "") { //Variable exists if not blank
					uint32_t variablelen = varvalue.length();
					//Substring
					int32_t start = 0;
					uint32_t len = -1; //max
					
					if(startindex != NULL) {
						char *lenindex = splitstring(startindex, ',');
						
						start = atoi(startindex);
						if(start < 0) start = variablelen + start;
						if(start < 0) start = 0; //make sure start not negative
						
						if(lenindex) {
							int32_t tempend = atoi(lenindex);
							len = (tempend < 0) ? variablelen + tempend : start + tempend;
						}
					}
					
					for(uint32_t i = 0 + start; (i < variablelen) && (i < len); i++) {
						buffer.push_back(varvalue[i]);
					}
				}
				std::vector<char>().swap(varnamebuffer); //Clean the vector
			}
			writevarname = !writevarname;
		} else if(writevarname) {
			varnamebuffer.push_back(*value);
		} else {
			buffer.push_back(*value);
			if(amount != NULL && *value == '!') (*amount)++;
		}
	}
	buffer.push_back('\0');
	return buffer.data();
}
