#include "neem.h"

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "set") == 0) return set_;
	return none_;
}

std::string Neem::parsevariablevalue(std::string *value) {
	return parsevariables(parsebuffer, value->c_str());
}

std::string Neem::parsevariables(char *buffer, const char *value) { //the index is a return index
	char varnamebuffer[MAX_VARNAME_LEN];
	uint8_t varnameindex;
	bool writevarname = false;
	
	uint16_t outindex = 0; //Needs to be outside the for scope for the \0
	std::map<const std::string, std::string>::iterator variableinter;
	
	for(uint16_t i = 0; value[i]; i++) {
		if(value[i] == '%') {
			if(writevarname) { //becoming false
				varnamebuffer[varnameindex] = '\0';
				if((variableinter = variables.find(varnamebuffer)) != variables.end()) { //Variable exists, so we put it in the buffer
					const char *variable = variableinter->second.c_str();
					while(*(variable)) {
						buffer[outindex++] = *(variable++); //could use strcpy but I want the output index to roll over; will continue until \0
					}
				}
			} else varnameindex = 0; //reset index
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

void Neem::parseline(char *line) {
	while(isspace((unsigned char) *line)) line++; //Remove leading spaces
	if(*line == '\0') return; //if, after spaces, it's blank, just return
	
	char *params = line + strlen(strtok(line, " "))+1; //strtok splits, then strlen gets where the \0 is, then add that "index" to the pointer
	{ //Scope this since instruction will just be put into the vector
		instruction i;
		i.type = gettype(line);
		if(i.type == none_) return; //This is so we can /this/ properly
		instructions.push_back(i);
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case echo_:
			last->value = std::string(params); //Create string and populate it
			last->func = [this](instruction *i, uint16_t index) { 
				printf("%s\n", parsevariablevalue(&i->value).c_str());
				return -1; //-1 is the 0 of this function; anything positive becomes the new line index
			};
			break;
		case set_:
			last->value = std::string(strtok(params, "="));
			last->extravalue = std::string(params + strlen(params)+1); //So the strtok returns a pointer to the variable; strlen gets where the \0 is, then +1 for the start of the next, then add this "index" to the char pointer
			last->func = [this](instruction *i, uint16_t index) {
				variables[parsevariablevalue(&i->value)] = parsevariablevalue(&i->extravalue);
				return -1; //parsevariablevalue(i)
			};
			break;
	};
}

void Neem::interpretFile(char *fname) {
	cleanup(); //Make sure everything is clean
	
	FILE *file;
	char linebuffer[MAX_LINE_LEN];
	uint16_t length; //tied to MAX_LINE_LEN
	
	if( (file = fopen(fname, "r")) == NULL) {
		printf("[!] Can't open '%s'\n", fname);
		return;
	}
	
	while (fgets(linebuffer, sizeof(linebuffer), file)) {
		length = strlen(linebuffer);
		for(uint16_t i = 0; i < length; i++) if(linebuffer[i] == '\r' || linebuffer[i] == '\n') linebuffer[i] = '\0'; // 'remove' chars we really don't want
		
		parseline(linebuffer);
	}
	
	fclose(file);
	
	int ret = 0;
	for(uint16_t i = 0, e = instructions.size(); i < e; i++) {
		instruction *current = &instructions.front() +i; //get the pointer to the element
		if((ret = current->func(current, i)) < -1) return; //Error
		else if(ret != -1) i = ret; // -1 is the good value here
	}
}

void Neem::cleanup() {
	variables.clear();
	instructions.clear();
	instructions.shrink_to_fit();
}

Neem::~Neem() {
	cleanup();
}
