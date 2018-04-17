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
	if(strcasecmp(command, "libload") == 0) return {libload_, &Neem::command_libload};
	if(strcasecmp(command, "libunload") == 0) return {libunload_, &Neem::command_libunload};
	if(strcasecmp(command, "librun") == 0) return {librun_, &Neem::command_librun};
	if(command[0] == '#' || command[0] == '-') return {comment_, NULL}; //Lua comments or shebang
	if(command[0] == ':' && command[1] != ':') return {label_, &Neem::command_none};
	if(command[0] == ':' && command[1] == ':') return {comment_, NULL};
	return {none_, NULL};
}

bool Neem::parseline(char *line, uint32_t index) {
	while(isspace((uint8_t) *line)) line++; //Remove leading spaces
	if(*line == '\0') return 1; //if, after spaces, it's blank, just return
	
	char *params = splitstring(line, ' ');
	
	{
		typeandfunc current = gettype(line);
		if(current.type == comment_) return true;
		else if(current.type == none_) {
			std::string l = line;
			alert('!', "'%s' is not a command", NULL, &l);
			return false;
		} else if(current.type != import_) { //Special command
			instruction i;
			i.type = current.type;
			i.func = current.func;
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
	
	switch(last->type) { //This is how the syntax is dealt with
		
		case exit_: //just command on a line: "rof" / "exit"
		case pwd_:
		case pause_:
		case else_:
		case fi_:
		case rof_:
			break;
		
		case echo_: // echo SOMETHING / echo
		case case_: // case SOMETHING / case
		case readline_: // readline variable_name / readline
			last->value = (params != NULL) ? params : ""; //value (optional)
			break;
		
		case label_: // :label
			last->value = line+1; //label name but with : removed
			break;

		case sleep_: // sleep 2000
		case start_: // start program_name
		case libload_: // loadlib library_name
		case libunload_: // unload library_name
		case switch_: // switch valuetocheck
		case input_:
		case readall_:
		case rm_:
		case rmdir_:
			if(params == NULL) {alert('!', "Missing parameters", &index); return false;}
			last->value = params;
			break;
			
		case sum_: //Input with equals: "set var=val"
		case strftime_:
		case prompt_:
		case setsystem_:
		case set_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			break;
			
		case goto_: //goto :label
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			break;
		
		case if_:
			last->extravalue = setifcheck(last, params); //sets the if function and returns after the op
			last->value = params; //setif \0s the left part
			break;
			
		case for_:
			{
				char *arrstring = splitstring(params, ' ');
				char *delim = splitstring(arrstring, ' ');
				last->xxxtravalue = (delim != NULL) ? delim : std::string(1,FORCHARCHECK); //The delimiter
				last->extravalue = arrstring; //input/arr var
				last->value = params; //index value var
			}
			break;

		case inc_:
			{
				char *temp = splitstring(params, '=');
				if(temp != NULL) last->extravalue = temp; //Number to add (optional)
				else last->extravalue = "";
				last->value = params;
			}
			break;

		case cd_:
			if(params == NULL) last->extravalue = "";
				else last->value = params; //Directory name
			break;
			
		case ls_:
			last->value = (params != NULL) ? params : ".";
			break;

		case librun_:
			{ //scope this bit
				char *temp = splitstring(params, ' ');
				last->xxxtravalue = splitstring(temp, ' '); //args
				last->extravalue = temp; //function
				last->value = params; //library
			}
			break;
			
		case output_:
			{
				char *temp = splitstring(params, ' ');
				last->value = params; //filename
				if(temp != NULL) last->extravalue = temp; //file attributes (optional)
				else last->extravalue = "a"; //default to append
			}
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
	scriptpath = "";
}

void Neem::cleanup() {
	variables.clear();
	
	for (auto it = loadedlibs.begin(); it != loadedlibs.end(); it++) {
		freelibrary(it->second);
	}
}
