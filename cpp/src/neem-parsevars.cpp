#include "neem.h"

std::string Neem::parsevariables(const char *value, const char searchfor, uint8_t *amount) { //the index is a return index
	const char *(*check)(std::map<const std::string, std::string> *,
						 std::map<const std::string, std::function<std::string()>> *, char *) = 
		[](std::map<const std::string, std::string> *variables,
		   std::map<const std::string, std::function<std::string()>> *globalvariables, char *findtarget) {
				std::map<const std::string, std::function<std::string()>>::iterator gvar;
				if((gvar = globalvariables->find(findtarget)) != globalvariables->end()) return gvar->second().c_str();
				std::map<const std::string, std::string>::iterator var;
				if((var = variables->find(findtarget)) != variables->end()) return var->second.c_str();
				return (const char *)NULL;
	};
	std::vector<char> buffer;
	std::vector<char> varnamebuffer;
	bool writevarname = false;
	
	for(; *value; value++) {
		if(*value == searchfor) {
			if(!writevarname && *(value+1) == searchfor) {
				buffer.push_back(searchfor);
				buffer.push_back(searchfor);
				value++;
				continue;
			}
			if(writevarname) { //becoming false
				
				varnamebuffer.push_back('\0');
				size_t varnamelen = varnamebuffer.size();
				char *varname = varnamebuffer.data();
				
				char *startindex = splitstring(varname, ':'); //for the substring
				if(startindex == NULL || *(startindex) != '~') startindex = NULL;
				const char *varvalue = check(&variables, &globalvariables, varname);
				
				if(varvalue != NULL) { //Variable exists, as a const char
					std::string tempvar = varvalue;
					uint32_t variablelen = tempvar.length();
					//Substring
					int32_t start = 0;
					uint32_t len = -1; //max
					
					if(startindex != NULL) {
						startindex++; //After the ~
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
