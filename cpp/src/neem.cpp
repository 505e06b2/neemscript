#include "neem.h"

Neem::Neem() { //Set up globals
	globalvariables["time"] = [this]() {
		return getstrftime(9, "%H:%M:%S");
	};
	
	globalvariables["date"] = [this]() {
		return getstrftime(9, "%d/%m/%y");
	};
	
	globalvariables["cd"] = [this]() {
		return getcurrentdir();
	};
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
	if(strcasecmp(command, "start") == 0) return start_;
	if(strcasecmp(command, "pwd") == 0) return pwd_;
	if(strcasecmp(command, "cd") == 0) return cd_;
	if(strcasecmp(command, "ls") == 0) return ls_;
	if(strcasecmp(command, "pause") == 0) return pause_;
	if(command[0] == ':' && command[1] != ':') return label_;
	if(command[0] == ':' && command[1] == ':') return comment_;
	return none_;
}

bool Neem::parseline(char *line) {
	while(isspace((unsigned char) *line)) line++; //Remove leading spaces
	if(*line == '\0') return true; //if, after spaces, it's blank, just return
	
	strtok(line, " ");
	char *params = strtok(NULL, "");
	{ //Scope this since instruction will just be put into the vector and we can minimise the memory used
		types currenttype = gettype(line);
		if(currenttype == comment_) return true; //This is so we can /this/ properly
		else if(currenttype == none_) {printf("[!] '%s' is not a command\n", line); return false;}
		instruction i;
		i.type = currenttype;
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
				std::this_thread::sleep_for(std::chrono::milliseconds( stoi(parsevarval(&i->value)) ));
				return -1;
			};
			break;
		case start_:
			last->value = params;
			last->func = [this](instruction *i, uint16_t index) {
				system(i->value.c_str());
				return -1;
			};
			break;
		case pwd_:
			last->func = [this](instruction *i, uint16_t index) {
				printf("%s\n", getcurrentdir().c_str());
				return -1;
			};
			break;
		case cd_:
			if(params == NULL) last->extravalue = "#";
				else last->value = params;
			last->func = [this](instruction *i, uint16_t index) {
				if(i->extravalue != "") printf("%s\n", getcurrentdir().c_str());
					else chdir(parsevarval(&i->value).c_str());
				return -1;
			};
			break;
		case ls_:
			last->func = [this](instruction *i, uint16_t index) {
				struct dirent *d;
				DIR *dir = opendir(".");
				while((d = readdir(dir)) != NULL)
					if(d->d_name[0] != '.') printf("%s\n", d->d_name);
				closedir(dir);
				return -1;
			};
			break;
		case pause_:
			last->func = [this](instruction *i, uint16_t index) {
				printf("Press ENTER to continue...");
				getchar();
				return -1;
			};
			break;
	};
	return true;
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
		for(uint16_t i = length, e = length-3; i > e; i--) {
			switch(linebuffer[i]) { // 'remove' chars we really don't want
				case '\r':
				case '\n':
					linebuffer[i] = '\0';
			}
		}
		
		if(!parseline(linebuffer)) return;
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
