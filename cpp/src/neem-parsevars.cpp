#include "neem.h"

std::string Neem::parsevariables(const char *value, const char searchfor, uint8_t *amount) { //the index is a return index
	std::function<std::string(std::map<const std::string, std::string> *,
						 std::map<const std::string, std::function<std::string(char *)>> *, char *)> check = 
		[this](std::map<const std::string, std::string> *variables,
		   std::map<const std::string, std::function<std::string(char *)>> *dynamicvariables, char *findtarget) {
				std::map<const std::string, std::function<std::string(char *)>>::iterator gvar;
				char *possibleparams = splitstring(findtarget, ';');
				if((gvar = dynamicvariables->find(findtarget)) != dynamicvariables->end()) return gvar->second(possibleparams);
				std::map<const std::string, std::string>::iterator var;
				if((var = variables->find(findtarget)) != variables->end()) return var->second;
				return (std::string)"";
		};
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
				std::string varvalue = check(&variables, &dynamicvariables, varname);
				
				if(varvalue != "") { //Variable exists if not blank
					std::string tempvar = varvalue;
					uint32_t variablelen = tempvar.length();
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
