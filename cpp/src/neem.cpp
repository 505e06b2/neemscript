#include "neem.h"

Neem::typeandfunc Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0 || strcasecmp(command, "print") == 0) return {echo_, &Neem::command_echo};
	if(strcasecmp(command, "setsystem") == 0) return {setsystem_, &Neem::command_setsystem};
	if(strcasecmp(command, "import") == 0) return {import_, NULL};
	if(strcasecmp(command, "set") == 0) return {set_, &Neem::command_set};
	if(strcasecmp(command, "prompt") == 0) return {prompt_, &Neem::command_prompt};
	if(strcasecmp(command, "if") == 0) return {if_, &Neem::command_if};
	if(strcasecmp(command, "else") == 0) return {else_, &Neem::command_else};
	if(strcasecmp(command, "fi") == 0) return {fi_, &Neem::command_none};
	if(strcasecmp(command, "switch") == 0) return {switch_, &Neem::command_switch};
	if(strcasecmp(command, "case") == 0) return {case_, &Neem::command_case};
	if(strcasecmp(command, "for") == 0) return {for_, &Neem::command_for};
	if(strcasecmp(command, "rof") == 0) return {rof_, &Neem::command_rof};
	if(strcasecmp(command, "sum") == 0) return {sum_, &Neem::command_sum};
	if(strcasecmp(command, "goto") == 0) return {goto_, &Neem::command_goto};
	if(strcasecmp(command, "call") == 0) return {call_, &Neem::command_call};
	if(strcasecmp(command, "inc") == 0) return {inc_, &Neem::command_inc};
	if(strcasecmp(command, "sleep") == 0) return {sleep_, &Neem::command_sleep};
	if(strcasecmp(command, "strftime") == 0) return {strftime_, &Neem::command_strftime};
	if(strcasecmp(command, "start") == 0) return {start_, &Neem::command_start};
	if(strcasecmp(command, "pwd") == 0) return {pwd_, &Neem::command_pwd};
	if(strcasecmp(command, "cd") == 0) return {cd_, &Neem::command_cd};
	if(strcasecmp(command, "rm") == 0) return {rm_, &Neem::command_rm};
	if(strcasecmp(command, "rmdir") == 0) return {rmdir_, &Neem::command_rmdir};
	if(strcasecmp(command, "ls") == 0) return {ls_, &Neem::command_ls};
	if(strcasecmp(command, "exit") == 0) return {exit_, &Neem::command_exit};
	if(strcasecmp(command, "pause") == 0) return {pause_, &Neem::command_pause};
	if(strcasecmp(command, "output") == 0) return {output_, &Neem::command_output};
	if(strcasecmp(command, "input") == 0) return {input_, &Neem::command_input};
	if(strcasecmp(command, "readall") == 0) return {readall_, &Neem::command_readall};
	if(strcasecmp(command, "readline") == 0) return {readline_, &Neem::command_readline};
	if(strcasecmp(command, "loadlib") == 0) return {loadlib_, &Neem::command_loadlib};
	if(strcasecmp(command, "unloadlib") == 0) return {unloadlib_, &Neem::command_unloadlib};
	if(strcasecmp(command, "runlibfunc") == 0) return {runlibfunc_, &Neem::command_runlibfunc};
	if(command[0] == '#' || command[0] == '-') return {comment_, NULL}; //Lua comments or shebang
	if(command[0] == ':' && command[1] != ':') return {label_, &Neem::command_none};
	if(command[0] == ':' && command[1] == ':') return {comment_, NULL};
	return {none_, NULL};
}

bool Neem::parseline(char *line) {
	while(isspace((uint8_t) *line)) line++; //Remove leading spaces
	if(*line == '\0') return 1; //if, after spaces, it's blank, just return
	
	char *params = splitstring(line, ' ');
	typeandfunc current = gettype(line);
	{
		if(current.type == comment_) return true;
		else if(current.type == none_) {
			std::string l = line;
			alert('!', "'%s' is not a command", NULL, &l);
			return false;
		} else if(current.type != import_) { //Special command
			instruction i;
			i.type = current.type;
			instructions.push_back(i);
		} else { //If import_
			if(params == NULL) {
				alert('!', "Can't import", NULL);
				return false;
			}
			if(!readfilebyline(params, &Neem::parseline)) return false;
			return true;
		}
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case echo_: // echo SOMETHING / echo
		case case_: // case SOMETHING / case
		case readline_: // readline variable_name / readline
			last->value = (params != NULL) ? params : ""; //value (optional)
			last->func = current.func;
			break;
		
		case sleep_: // sleep 2000
		case start_: // start program_name
		case loadlib_: // loadlib library_name
		case unloadlib_: // unload library_name
		case switch_: // switch valuetocheck
		case input_:
		case readall_:
		case rm_:
		case rmdir_:
			last->value = params;
			last->func = current.func;
			break;
		
		case label_: // :label
			last->value = line+1; //label name but with : removed
		//CONTINUE DOWNWARDS AFTER
		case exit_: //just command on a line: "rof" / "exit"
		case pwd_:
		case pause_:
		case else_:
		case fi_:
		case rof_:
			last->func = current.func;
			break;
			
		case sum_: //Input with equals: "set var=val"
		case strftime_:
		case prompt_:
		case setsystem_:
		case set_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			last->func = current.func;
			break;
			
		case goto_: //goto :label
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = current.func;
			break;
		
		case if_:
			last->extravalue = setifcheck(last, params); //sets the if function and returns after the op
			last->value = params; //setif \0s the left part
			last->func = &Neem::command_if;
			break;
			
		case for_:
			{
				char *arrstring = splitstring(params, ' ');
				char *delim = splitstring(arrstring, ' ');
				last->xxxtravalue = (delim != NULL) ? delim : std::string(1,FORCHARCHECK); //The delimiter
				last->extravalue = arrstring; //input/arr var
				last->value = params; //index value var
			}
			last->func = &Neem::command_for;
			break;

		case inc_:
			{
				char *temp = splitstring(params, '=');
				if(temp != NULL) last->extravalue = temp; //Number to add (optional)
				else last->extravalue = "";
				last->value = params;
			}
			last->func = &Neem::command_inc;
			break;

		case cd_:
			if(params == NULL) last->extravalue = "";
				else last->value = params; //Directory name
			last->func = &Neem::command_cd;
			break;
			
		case ls_:
			last->value = (params != NULL) ? params : ".";
			last->func = &Neem::command_ls;
			break;

		case runlibfunc_:
			{ //scope this bit
				char *temp = splitstring(params, ' ');
				last->xxxtravalue = splitstring(temp, ' '); //args
				last->extravalue = temp; //function
				last->value = params; //library
			}
			last->func = &Neem::command_runlibfunc;
			break;
			
		case output_:
			{
				char *temp = splitstring(params, ' ');
				last->value = params; //filename
				if(temp != NULL) last->extravalue = temp; //file attributes (optional)
				else last->extravalue = "a"; //default to append
			}
			last->func = &Neem::command_output;
			break;
	};
	return true;
}

void Neem::runInstructions() {
	int ret = 0;
	for(uint16_t i = 0, e = instructions.size(); i < e; i++) {
		instruction *current = &instructions.front() +i; //get the pointer to the element
		if((ret = (this->*current->func)(current, i)) < -1) return; //Error
		else if(ret != -1) i = ret; // -1 is the good value here
	}
	
	//Cleaning
	if(eof != (uint16_t)-2) {
		std::string index = std::to_string(eof+1);
		alert('#', "Called without 'goto :eof'; check the label referenced by line: %s", NULL, &index);
		eof = -2; //Make sure we fix it so nothing weird happens next run
	}
	
	if(inputhandle != NULL) {
		fclose(inputhandle);
		inputhandle = NULL;
	}
	
	if(outputhandle != stdout) {
		fclose(outputhandle);
		outputhandle = stdout;
	}
	std::vector<instruction>().swap(instructions); //Remove all instructions that we just ran
	switchcheckstring = "";
}

void Neem::cleanup() {
	variables.clear();
	
	for (auto it = loadedlibs.begin(); it != loadedlibs.end(); it++) {
		freelibrary(it->second);
	}
}
