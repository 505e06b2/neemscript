#include "neem.h"

Neem::Neem() { //Set up globals
	globalvariables["time"] = [this]() {
		return getstrftime(9, "%H:%M:%S");
	};
	
	globalvariables["date"] = [this]() {
		return getstrftime(9, "%d/%m/%y");
	};
}

std::string Neem::getstrftime(size_t size, const char *format) {
	struct tm *tm;
	char str_date[size];
	time_t t = time(NULL);
	tm = localtime(&t);
	strftime(str_date, sizeof(str_date), format, tm);
	return str_date;
}

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "set") == 0) return set_;
	if(strcasecmp(command, "if") == 0) return if_;
	if(strcasecmp(command, "fi") == 0) return fi_;
	if(strcasecmp(command, "goto") == 0) return goto_;
	if(strcasecmp(command, "call") == 0) return call_;
	if(strcasecmp(command, "inc") == 0) return inc_;
	if(strcasecmp(command, "sleep") == 0) return sleep_;
	if(strcasecmp(command, "strftime") == 0) return strftime_;
	if(command[0] == ':' && command[1] != ':') return label_;
	return none_;
}

std::string Neem::parsevarval(std::string *value) {
	return parsevariables(parsebuffer, value->c_str());
}

std::string Neem::parsevariables(char *buffer, const char *value) { //the index is a return index
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

char *Neem::setifcheck(instruction *i, char *string) {
	while(*string) {
		switch(*string) {
			
			case '<':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(stof(left) <= stof(right)) return true; return false; };
					return (string+2);
				} else {
					i->check = [](std::string left, std::string right) { if(stof(left) < stof(right)) return true; return false; };
					return (string+1);
				}
				
			case '>':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(stof(left) >= stof(right)) return true; return false; };
					return (string+2);
				} else {
					i->check = [](std::string left, std::string right) { if(stof(left) > stof(right)) return true; return false; };
					return (string+1);
				}
				
			case '!':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(left != right) return true; return false; };
					return (string+2);
				} else { //Case insensitive !equals
					i->check = [](std::string left, std::string right) { if(strcasecmp(left.c_str(), right.c_str()) != 0) return true; return false; };
					return (string+1);
				}
				
			case '=':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(left == right) return true; return false; };
					return (string+2);
				} else { //Case insensitive !equals
					i->check = [](std::string left, std::string right) { if(strcasecmp(left.c_str(), right.c_str()) == 0) return true; return false; };
					return (string+1);
				}
		}
		string++;
	}
	printf("[!] If string not correct: %s\n", string);
}

void Neem::parseline(char *line) {
	while(isspace((unsigned char) *line)) line++; //Remove leading spaces
	if(*line == '\0') return; //if, after spaces, it's blank, just return
	
	strtok(line, " ");
	char *params = strtok(NULL, "");
	{ //Scope this since instruction will just be put into the vector and we can minimise the memory used
		instruction i;
		i.type = gettype(line);
		if(i.type == none_) return; //This is so we can /this/ properly
		instructions.push_back(i);
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case echo_:
			last->value = params;
			last->func = [this](instruction *i, uint16_t index) { 
				printf("%s\n", parsevarval(&i->value).c_str());
				return -1; //-1 is the 0 of this function; anything positive becomes the new line index
			};
			break;
		case set_:
			last->value = strtok(params, "="); //So the strtok returns a pointer to the variable; 
			last->extravalue = std::string(strtok(NULL, ""));
			last->func = [this](instruction *i, uint16_t index) {
				variables[parsevarval(&i->value)] = parsevarval(&i->extravalue);
				return -1;
			};
			break;
		case if_:
			last->extravalue = setifcheck(last, params); //extra needs to be first since it's the right side
			last->value = params; //setif \0s the left part
			last->func = [this](instruction *i, uint16_t index) {
				if(!i->check( parsevarval(&i->value), parsevarval(&i->extravalue) )) {
					for(uint16_t e = instructions.size(); index < e; index++)
						if(instructions[index].type == fi_) return (int)index;
				}
				return -1;
			};
			break;
		case goto_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint16_t index) {
				if(i->value == "eof") {
					uint16_t tempeof = eof;
					eof = -2; //-2 or it overflows
					return (int)tempeof;
				}
				for(uint16_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == i->value) return (int)index;
				}
				printf("[!] %d:Can't goto %s", index+1, i->value.c_str());
				return -1;
			};
			break;
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint16_t index) {
				eof = index;
				for(uint16_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == i->value) return (int)index;
				}
				printf("[!] %d:Can't call %s", index+1, i->value.c_str());
				return -1;
			};
			break;
		case inc_:
			last->value = params;
			last->func = [this](instruction *i, uint16_t index) {
				std::map<const std::string, std::string>::iterator variableinter;
				std::string var = parsevarval(&i->value);
				if((variableinter = variables.find(var)) != variables.end()) { //Variable exists, so we get it from the map
					variables[var] = std::to_string(stoi(variables[var]) + 1);
				} else {
					printf("[!] %d:Can't inc %s\n", index+1, var.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case label_:
			last->value = line+1;
			break;
		case fi_:
			break;
		case strftime_:
			last->value = strtok(params, "="); //So the strtok returns a pointer to the variable; 
			last->extravalue = std::string(strtok(NULL, ""));
			last->func = [this](instruction *i, uint16_t index) {
				variables[parsevarval(&i->value)] = getstrftime(64, i->extravalue.c_str());
				return -1;
			};
			break;
		case sleep_:
			last->value = params;
			last->func = [this](instruction *i, uint16_t index) {
				uint32_t ms = stoi(parsevarval(&i->value));
				struct timespec ts;
				ts.tv_sec = ms / 1000;
				ts.tv_nsec = (ms % 1000) * 1000000;
				nanosleep(&ts, NULL);
				return -1;
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
		for(uint16_t i = 0; i < length; i++) {
			switch(linebuffer[i]) { // 'remove' chars we really don't want
				case '\r':
				case '\n':
					linebuffer[i] = '\0';
			}
		}
		
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
