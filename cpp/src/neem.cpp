#include "neem.h"

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "print") == 0) return echo_;
	if(strcasecmp(command, "setsystem") == 0) return setsystem_;
	if(strcasecmp(command, "set") == 0) return set_;
	if(strcasecmp(command, "prompt") == 0) return prompt_;
	if(strcasecmp(command, "if") == 0) return if_;
	if(strcasecmp(command, "else") == 0) return else_;
	if(strcasecmp(command, "fi") == 0) return fi_;
	if(strcasecmp(command, "for") == 0) return for_;
	if(strcasecmp(command, "rof") == 0) return rof_;
	if(strcasecmp(command, "sum") == 0) return sum_;
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

bool Neem::parseline(char *line) {
	while(isspace((uint8_t) *line)) line++; //Remove leading spaces
	if(*line == '\0') return 1; //if, after spaces, it's blank, just return
	
	char *params = splitstring(line, ' ');
	{ //Scope this since instruction will just be put into the vector and we can minimise the memory used
		types currenttype = gettype(line);
		if(currenttype == comment_) return true; //This is so we can /this/ properly
		else if(currenttype == none_) {
			std::string l = line;
			alert('!', "'%s' is not a command", NULL, &l);
			return false;
		}
		instruction i;
		i.type = currenttype;
		instructions.push_back(i);
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case fi_:
			break;
		case echo_:
			last->value = (params != NULL) ? params : ""; //value to print
			last->func = [this](instruction *i, uint32_t index) {
				fprintf(outputhandle, "%s\n", parsevarval(&i->value).c_str());
				return -1; //-1 is the 0 of this function; anything positive becomes the new line index
			};
			break;
		case setsystem_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			last->func = [this](instruction *i, uint32_t index) {
				parsedstrings parsed;
				parseallstrings(&parsed, i);
				if(!setenvvar(&parsed.value, &parsed.extravalue)) {
					alert('!', "Can't put '%s' into an environment variable", &index, &parsed.extravalue);
					return -2;
				}
				return -1;
			};
			break;
		case set_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = parsevarval(&i->extravalue);
				return -1;
			};
			break;
		case prompt_: //type input
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
		case sum_:
			last->extravalue = splitstring(params, '='); //Calculation
			last->value = params; //variable
			last->func = [this](instruction *i, uint32_t index) {
				variables[parsevarval(&i->value)] = std::to_string(dosum(parsevarval(&i->extravalue).c_str()));
				return -1;
			};
			break;
		case if_:
			last->extravalue = setifcheck(last, params); //sets the if function and returns after the op
			last->value = params; //setif \0s the left part
			last->func = [this](instruction *i, uint32_t index) {
				if(!i->check( parsevarval(&i->value), parsevarval(&i->extravalue) )) {
					int ret = searchfortag(&index, else_, if_);
					if(ret >= 0) return ret;
					ret = searchfortag(&index, fi_, if_);
					if(ret >= 0) return ret;
					return alert('!', "No matching 'fi' for if", &index);
				}
				return -1;
			};
			break;
		case else_:
			last->func = [this](instruction *i, uint32_t index) {
				int ret = searchfortag(&index, fi_, if_);
				if(ret >= 0) return ret;
				return alert('!', "No matching 'fi' for else", &index);
			};
			break;
		case for_:
			{
				char *arrstring = splitstring(params, ' ');
				char *delim = splitstring(arrstring, ' ');
				last->xxxtravalue = (delim != NULL) ? delim : std::string(1,FORCHARCHECK); //The delimiter
				last->extravalue = arrstring; //input/arr var
				last->value = params; //index value var
			}
			last->func = [this](instruction *i, uint32_t index) {
				parsedstrings parsed;
				parseallstrings(&parsed, i);
				const char *therest = variables[parsed.extravalue].c_str();
				std::string currentvalue = "";
				
				for(; *therest; therest++) {
					if(strncmp(therest, parsed.xxxtravalue.c_str(), parsed.xxxtravalue.size()) == 0) {
						therest += parsed.xxxtravalue.size();
						break;
					} else {
						currentvalue += *therest;
					}
				}
				
				if(currentvalue == "") { //Loop done
					int ret = searchfortag(&index, rof_, for_);
					if(ret >= 0) return ret;
					return alert('!', "No matching 'rof' for For loop", &index);
				}
				
				variables[parsed.extravalue] = therest;
				variables[parsed.value] = currentvalue;
				return -1;
			};
			break;
		case rof_:
			last->func = [this](instruction *i, uint32_t index) {
				uint8_t forloops = 1;
				for(int32_t a = index; a >= 0; a--)
					if(instructions[a].type == for_) {
						if(--forloops == 0) return (a-1); //-1 or it will go to the line after
					} else if(instructions[a].type == for_) {
						forloops++;
					}
				return alert('!', "No matching 'for' for rof", &index);
			};
			break;
		case goto_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				if(parsed == "eof") {
					uint32_t tempeof = eof;
					eof = -2; //-2 or it overflows
					return (int)tempeof;
				}
				for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == parsed) return (int)index;
				}
				return alert('!', "Can't goto '%s'", &index, &parsed);
			};
			break;
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				eof = index;
				for(uint32_t index = 0, e = instructions.size(); index < e; index++) {
					if(instructions[index].type == label_ && instructions[index].value == parsed) return (int)index;
				}
				return alert('!', "Can't call '%s'", &index, &parsed);
			};
			break;
		case inc_:
			{
				char *temp = splitstring(params, '=');
				if(temp != NULL) last->extravalue = temp; //Number to add (optional)
				else last->extravalue = "";
				last->value = params;
			}
			last->func = [this](instruction *i, uint32_t index) {
				std::map<const std::string, std::string>::iterator variableinter;
				parsedstrings parsed;
				parseallstrings(&parsed, i);
				if((variableinter = variables.find(parsed.value)) != variables.end()) { //Variable exists, so we get it from the map
					int increaseby = 1;
					if(i->extravalue != "") {
						if(parsed.extravalue != "") increaseby = stoi(parsed.extravalue);
						else {
							increaseby = 0;
							alert('#', "'%s' isn't defined", &index, &parsed.value);
						}
					}
					variables[parsed.value] = std::to_string(stoi(variables[parsed.value]) + increaseby);
				} else {
					return alert('!', "Can't inc '%s'", &index, &parsed.value);
				}
				return -1;
			};
			break;
		case label_:
			last->value = line+1; //label name but with : removed
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
			last->value = params; //Time to sleep
			last->func = [this](instruction *i, uint32_t index) {
				std::this_thread::sleep_for(std::chrono::milliseconds( stoi(parsevarval(&i->value)) ));
				return -1;
			};
			break;
		case start_:
			last->value = params; //Program name
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				FILE *f = popen(parsed.c_str(), "r");
				FILE *stderrbackup = stderr;
				if(f == NULL) {
					return alert('!', "Could not open program: '%s'", &index, &parsed);
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
			if(params == NULL) last->extravalue = "";
				else last->value = params; //Directory name
			last->func = [this](instruction *i, uint32_t index) {
				if(i->extravalue != "") fprintf(outputhandle, "%s\n", getcurrentdir().c_str()); //Print if no args
					else chdir(parsevarval(&i->value).c_str());
				return -1;
			};
			break;
		case ls_:
			last->value = (params != NULL) ? params : ".";
			last->func = [this](instruction *i, uint32_t index) {
				fprintf(outputhandle, "%s\n", listdir(parsevarval(&i->value).c_str(), '\n').c_str());
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
			last->value = params; //libname
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				if(!loadlibrary(parsed.c_str(), parsed.size())) return alert('!', "Could not load library: '%s'", &index, &parsed);
				return -1;
			};
			break;
		case unloadlib_:
			last->value = params; //lib
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value).c_str();
				auto it = loadedlibs.find(parsed);
				if(it == loadedlibs.end()) return alert('#', "Library not loaded: '%s'", &index, &parsed);
				freelibrary(it->second);
				loadedlibs.erase(it); //Remove from map
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
				parsedstrings parsed;
				parseallstrings(&parsed, i);
				if(loadedlibs.find(parsed.value) == loadedlibs.end()) return alert('!', "Library not loaded: '%s'", &index, &parsed.value);
				int ret = runlibraryfunction(&parsed.value, parsed.extravalue.c_str(), parsed.xxxtravalue.c_str());
				if(ret == -27202) return alert('!', "Could not load '%s' in: %s", &index, &parsed.extravalue, &parsed.value);
				else if(ret != 0) return alert('#', "Error in function: '%s'", &index, &parsed.extravalue);
				return -1;
			};
			break;
		case output_:
			{
				char *temp = splitstring(params, ' ');
				last->value = params; //filename
				if(temp != NULL) last->extravalue = temp; //file attributes (optional)
				else last->extravalue = "a"; //default to append
			}
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				if(outputhandle != stdout) fclose(outputhandle);
				if(parsed == "reset") outputhandle = stdout;
				else outputhandle = fopen(parsed.c_str(), parsevarval(&i->extravalue).c_str());
				if(outputhandle == NULL) return alert('!', "Could not open file: '%s'", &index, &parsed);
				return -1;
			};
			break;
		case input_:
			last->value = params; //filename
			last->func = [this](instruction *i, uint32_t index) {
				std::string parsed = parsevarval(&i->value);
				if(parsed == "reset") inputhandle = NULL;
				else inputhandle = fopen(parsed.c_str(), "r");
				if(inputhandle == NULL) return alert('!', "Could not open file: '%s'", &index, &parsed);
				return -1;
			};
			break;
		case readall_:
			last->value = params; //Variable name
			last->func = [this](instruction *i, uint32_t index) {
				if(inputhandle == NULL) return alert('!', "No input file loaded; use the input command", &index);
				char buffer[MAX_LINE_LEN];
				std::string parsed = parsevarval(&i->value);
				variables[parsed] = "";
				while(fread(buffer, sizeof(char), sizeof(buffer), inputhandle)) variables[parsed] += buffer;
				return -1;
			};
			break;
		case readline_:
			last->value = (params) ? params : ""; //Variable name (optional)
			last->func = [this](instruction *i, uint32_t index) {
				if(inputhandle == NULL) return alert('!', "No input file loaded; use the input command", &index);
				char buffer[MAX_LINE_LEN];
				std::string parsed = parsevarval(&i->value);
				if(fgets(buffer, sizeof(buffer), inputhandle) == NULL) {
					variables[parsed] = "";
				} else {
					if(i->value == "") return -1; //Put the line into a variable
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

void Neem::runInstructions() {
	int ret = 0;
	for(uint16_t i = 0, e = instructions.size(); i < e; i++) {
		instruction *current = &instructions.front() +i; //get the pointer to the element
		if((ret = current->func(current, i)) < -1) return; //Error
		else if(ret != -1) i = ret; // -1 is the good value here
	}
	std::vector<instruction>().swap(instructions); //Remove all instructions that we just ran
}

void Neem::cleanup() {
	if(outputhandle != stdout) fclose(outputhandle);
	variables.clear();
	
	for (auto it = loadedlibs.begin(); it != loadedlibs.end(); it++) {
		freelibrary(it->second);
	}
}
