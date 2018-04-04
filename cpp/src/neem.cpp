#include "neem.h"

Neem::Neem() { //Set up globals
	globalvariables["TIME"] = [this]() {
		return getstrftime(9, "%H:%M:%S");
	};
	
	globalvariables["DATE"] = [this]() {
		return getstrftime(9, "%d/%m/%y");
	};
	
	globalvariables["CD"] = [this]() {
		return getcurrentdir();
	};
	
	globalvariables["PATH"] = [this]() {
		return getenv("PATH");
	};
}

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "setsystem") == 0) return setsystem_;
	if(strcasecmp(command, "set") == 0) return set_;
	if(strcasecmp(command, "getsystem") == 0) return getsystem_;
	if(strcasecmp(command, "get") == 0) return get_;
	if(strcasecmp(command, "if") == 0) return if_;
	if(strcasecmp(command, "fi") == 0) return fi_;
	if(strcasecmp(command, "sum") == 0) return sum_;
	if(strcasecmp(command, "goto") == 0) return goto_;
	if(strcasecmp(command, "call") == 0) return call_;
	if(strcasecmp(command, "inc") == 0) return inc_;
	if(strcasecmp(command, "sleep") == 0) return sleep_;
	if(strcasecmp(command, "epoch") == 0) return epoch_;
	if(strcasecmp(command, "strftime") == 0) return strftime_;
	if(strcasecmp(command, "start") == 0) return start_;
	if(strcasecmp(command, "pwd") == 0) return pwd_;
	if(strcasecmp(command, "cd") == 0) return cd_;
	if(strcasecmp(command, "ls") == 0) return ls_;
	if(strcasecmp(command, "pause") == 0) return pause_;
	if(strcasecmp(command, "output") == 0) return output_;
	if(strcasecmp(command, "input") == 0) return input_;
	if(strcasecmp(command, "readall") == 0) return readall_;
	if(strcasecmp(command, "readline") == 0) return readline_;
	if(strcasecmp(command, "loadlib") == 0) return loadlib_;
	if(strcasecmp(command, "unloadlib") == 0) return unloadlib_;
	if(strcasecmp(command, "runlibfunc") == 0) return runlibfunc_;
	if(command[0] == '#') return comment_;
	if(command[0] == ':' && command[1] != ':') return label_;
	if(command[0] == ':' && command[1] == ':') return comment_;
	return none_;
}

bool Neem::parseline(char *line, uint32_t index) {
	while(isspace((uint8_t) *line)) line++; //Remove leading spaces
	if(*line == '\0') return 1; //if, after spaces, it's blank, just return
	
	char *params = splitstring(line, ' ');
	{ //Scope this since instruction will just be put into the vector and we can minimise the memory used
		types currenttype = gettype(line);
		if(currenttype == comment_) return true; //This is so we can /this/ properly
		else if(currenttype == none_) {fprintf(stderr, "[!] '%s' is not a command\n", line); return false;}
		instruction i;
		i.type = currenttype;
		instructions.push_back(i);
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case echo_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				fprintf(outputhandle, "%s\n", parsevarval(&i->value).c_str());
				return -1; //-1 is the 0 of this function; anything positive becomes the new line index
			};
			break;
		case setsystem_:
			last->extravalue = splitstring(params, '=');
			last->value = params; //varname
			last->func = [this](instruction *i, uint32_t index) {
				std::string name = parsevarval(&i->value);
				std::string value = parsevarval(&i->extravalue);
				if(!setenvvar(&name, &value)) {
					fprintf(stderr, "[!] %d:Can't put '%s' into an environment variable\n", index+1, value.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case set_:
			last->extravalue = splitstring(params, '=');
			last->value = params; //varname
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = parsevarval(&i->extravalue);
				return -1;
			};
			break;
		case get_: //type input
			last->extravalue = splitstring(params, '='); //Prompt
			last->value = params; //variable
			last->func = [this](instruction *i, uint32_t index) {
				char buff[128];
				fprintf(outputhandle, "%s", parsevarval(&i->extravalue).c_str());
				fgets(buff, sizeof(buff), stdin);
				buff[strlen(buff)-1] = '\0'; //remove \n
				variables[parsevarval(&i->value)] = buff;
				return -1;
			};
			break;
		case getsystem_: //get system var
			last->extravalue = splitstring(params, '='); //System var
			last->value = params; //variable
			last->func = [this](instruction *i, uint32_t index) {
				const char *val = getenv(parsevarval(&i->extravalue).c_str());
				std::string parsed = parsevarval(&i->value);
				if(val == NULL) {
					variables[parsed] = ""; //Blank is our NULL
				} else if(val[0] == '\0') {
					variables[parsed] = " "; //To show it exists but is just blank
				} else {
					variables[parsed] = val;
				}
				return -1;
			};
			break;
		case sum_:
			last->extravalue = splitstring(params, '='); //Calculation
			last->value = params; //variable
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = std::to_string(dosum(parsevarval(&i->extravalue).c_str()));
				return -1;
			};
			break;
		case if_:
			last->extravalue = setifcheck(last, params); //extra needs to be first since it's the right side
			last->value = params; //setif \0s the left part
			last->func = [this](instruction *i, uint32_t index) {
				if(!i->check( parsevarval(&i->value), parsevarval(&i->extravalue) )) {
					for(uint32_t e = instructions.size(); index < e; index++)
						if(instructions[index].type == fi_) return (int)index;
				}
				return -1;
			};
			break;
		case goto_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint32_t index) {
				std::string val = parsevarval(&i->value);
				if(val == "eof") {
					uint32_t tempeof = eof;
					eof = -2; //-2 or it overflows
					return (int)tempeof;
				}
				for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == val) return (int)index;
				}
				fprintf(stderr, "[!] %d:Can't goto %s", index+1, i->value.c_str());
				return -1;
			};
			break;
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint32_t index) {
				std::string val = parsevarval(&i->value);
				eof = index;
				for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == val) return (int)index;
				}
				fprintf(stderr, "[!] %d:Can't call %s", index+1, val.c_str());
				return -1;
			};
			break;
		case inc_:
			{
				char *temp = splitstring(params, '=');
				if(temp != NULL) last->extravalue = temp;
				else last->extravalue = "#";
				last->value = params;
			}
			last->func = [this](instruction *i, uint32_t index) {
				std::map<const std::string, std::string>::iterator variableinter;
				std::string var = parsevarval(&i->value);
				if((variableinter = variables.find(var)) != variables.end()) { //Variable exists, so we get it from the map
					int increaseby = 1;
					if(i->extravalue != "#") {
						std::string parsed = parsevarval(&i->extravalue);
						if(parsed != "") increaseby = stoi(parsed);
						else {
							increaseby = 0;
							fprintf(stderr, "[#] %d:%s isn't defined\n", index+1, i->extravalue.c_str());
						}
					}
					variables[var] = std::to_string(stoi(variables[var]) + increaseby);
				} else {
					fprintf(stderr, "[!] %d:Can't inc %s\n", index+1, var.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case epoch_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = std::to_string(time(NULL));
				return -1;
			};
			break;
		case label_:
			last->value = line+1;
			break;
		case fi_:
			break;
		case strftime_:
			last->extravalue = splitstring(params, '='); //strftime
			last->value = params; //varname
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = getstrftime(256, parsevarval(&i->extravalue).c_str());
				return -1;
			};
			break;
		case sleep_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				std::this_thread::sleep_for(std::chrono::milliseconds( stoi(parsevarval(&i->value)) ));
				return -1;
			};
			break;
		case start_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				FILE *f = popen(i->value.c_str(), "r");
				FILE *stderrbackup = stderr;
				if(f == NULL) {
					fprintf(stderr, "[!] %d:Could not open program: %s\n", index+1, i->value.c_str());
					return -2;
				}
				
				char buffer[MAX_LINE_LEN];
				while(fgets(buffer, MAX_LINE_LEN, f) != NULL) fprintf(outputhandle, "%s", buffer);
				return -1;
			};
			break;
		case pwd_:
			last->func = [this](instruction *i, uint32_t index) {
				fprintf(outputhandle, "%s\n", getcurrentdir().c_str());
				return -1;
			};
			break;
		case cd_:
			if(params == NULL) last->extravalue = "#";
				else last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				if(i->extravalue != "") fprintf(outputhandle, "%s\n", getcurrentdir().c_str()); //Print if no args
					else chdir(parsevarval(&i->value).c_str());
				return -1;
			};
			break;
		case ls_:
			last->func = [this](instruction *i, uint32_t index) {
				struct dirent *d;
				DIR *dir = opendir(".");
				while((d = readdir(dir)) != NULL)
					if(d->d_name[0] != '.') fprintf(outputhandle, "%s\n", d->d_name);
				closedir(dir);
				return -1;
			};
			break;
		case pause_:
			last->func = [this](instruction *i, uint32_t index) {
				printf("Press ENTER to continue...");
				getchar();
				return -1;
			};
			break;
		case loadlib_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				if(!loadlibrary(parsed.c_str(), parsed.size())) {
					fprintf(stderr, "[!] %d:Could not load library: %s\n", index+1, parsed.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case unloadlib_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value).c_str();
				auto it = loadedlibs.find(parsed);
				if(it == loadedlibs.end()) {fprintf(stderr, "[#] %d:Library not loaded: %s\n", index+1, parsed); return -1;}
				freelibrary(it->second);
				return -1;
			};
			break;
		case runlibfunc_:
			{ //scope this bit
				char *temp = splitstring(params, ' ');
				last->xxxtravalue = splitstring(temp, ' '); //args
				last->extravalue = temp; //function
				last->value = params; //library
			}
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsedval = parsevarval(&i->value);
				std::string extraparsedval = parsevarval(&i->extravalue);
				if(loadedlibs.find(parsedval) == loadedlibs.end()) {fprintf(stderr, "[!] %d:Library '%s' not loaded\n", index+1, parsedval.c_str()); return -2;}
				int ret = runlibraryfunction(&parsedval, extraparsedval.c_str(), parsevarval(&i->xxxtravalue).c_str());
				if(ret == -27202) {fprintf(stderr, "[!] %d:Could not load '%s' in: %s\n", index+1, extraparsedval.c_str(), parsedval.c_str()); return -2;}
				else if(ret != 0) {fprintf(stderr, "[#] %d:Error in function: %s\n", index+1, extraparsedval.c_str()); return -1;}
				return -1;
			};
			break;
		case output_:
			{
				char *temp = splitstring(params, ' ');
				last->value = params; //filename
				if(temp != NULL) last->extravalue = temp; //file attributes
				else last->extravalue = "a"; //default to append
			}
			last->func = [this](instruction *i, uint32_t index) {
				std::string val = parsevarval(&i->value);
				if(outputhandle != stdout) fclose(outputhandle);
				if(val == "reset") outputhandle = stdout;
				else outputhandle = fopen(val.c_str(), parsevarval(&i->extravalue).c_str());
				if(outputhandle == NULL) {
					fprintf(stderr, "[!] %d:Could not open file: %s\n", index+1, val.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case input_:
			last->value = params; //filename
			last->func = [this](instruction *i, uint32_t index) {
				std::string val = parsevarval(&i->value);
				if(val == "reset") inputhandle = NULL;
				else inputhandle = fopen(val.c_str(), "r");
				if(inputhandle == NULL) {
					fprintf(stderr, "[!] %d:Could not open file: %s\n", index+1, val.c_str());
					return -2;
				}
				return -1;
			};
			break;
		case readall_:
			last->value = params;
			last->func = [this](instruction *i, uint32_t index) {
				if(inputhandle == NULL) {
					fprintf(stderr, "[!] %d:No input file loaded; use the input command\n", index+1);
					return -2;
				}
				char buffer[1024];
				variables[parsevarval(&i->value)] = "";
				while(fread(buffer, sizeof(char), sizeof(buffer), inputhandle)) variables[parsevarval(&i->value)] += buffer;
				return -1;
			};
			break;
		case readline_:
			last->value = (params) ? params : "#";
			last->func = [this](instruction *i, uint32_t index) {
				if(inputhandle == NULL) {
					fprintf(stderr, "[!] %d:No input file loaded; use the input command\n", index+1);
					return -2;
				}
				char buffer[1024];
				std::string parsed = parsevarval(&i->value);
				if(fgets(buffer, sizeof(buffer), inputhandle) == NULL) {
					variables[parsed] = "";
				} else {
					if(i->value == "#") return -1;
					variables[parsed] = "";
					uint16_t len = strlen(buffer);
					for(int index = len; index > len-3 && index >= 0; index--) {
						if(buffer[index] == '\r' || buffer[index] == '\n') {
							buffer[index] = '\0';
						}
					}
					variables[parsed] = (buffer[0] == '\0') ? " " : buffer;
				}
				return -1;
			};
			break;
	};
	return true;
}

void Neem::interpretFile(char *fname) {
	FILE *file;
	char linebuffer[MAX_LINE_LEN];
	uint16_t length; //tied to MAX_LINE_LEN
	
	if((file = fopen(fname, "r")) == NULL) {
		fprintf(outputhandle, "[!] Can't open '%s'\n", fname);
		return;
	}
	

	for(uint32_t index = 0; fgets(linebuffer, sizeof(linebuffer), file); index++) {
		length = strlen(linebuffer);
		for(int i = length, e = length-3; i > e && i >= 0; i--) {
			switch(linebuffer[i]) { // 'remove' chars we really don't want
				case '\r':
				case '\n':
					linebuffer[i] = '\0';
			}
		}
		
		if(!parseline(linebuffer, index)) return;
	}
	
	fclose(file);
	
	int ret = 0;
	for(uint16_t i = 0, e = instructions.size(); i < e; i++) {
		instruction *current = &instructions.front() +i; //get the pointer to the element
		if((ret = current->func(current, i)) < -1) return; //Error
		else if(ret != -1) i = ret; // -1 is the good value here
	}
	
	fclose(outputhandle); //Should be fine even if it's stdout
	cleanup(); //make sure we're clean
}

void Neem::cleanup() {
	variables.clear();
	instructions.clear();
	instructions.shrink_to_fit();
	
	for (auto it = loadedlibs.begin(); it != loadedlibs.end(); it++) {
		freelibrary(it->second);
	}
	
}

Neem::~Neem() {
	cleanup();
}
