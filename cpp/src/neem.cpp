#include "neem.h"

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "print") == 0) return echo_;
	if(strcasecmp(command, "setsystem") == 0) return setsystem_;
	if(strcasecmp(command, "import") == 0) return import_;
	if(strcasecmp(command, "set") == 0) return set_;
	if(strcasecmp(command, "prompt") == 0) return prompt_;
	if(strcasecmp(command, "if") == 0) return if_;
	if(strcasecmp(command, "else") == 0) return else_;
	if(strcasecmp(command, "fi") == 0) return fi_;
	if(strcasecmp(command, "switch") == 0) return switch_;
	if(strcasecmp(command, "case") == 0) return case_;
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
	if(strcasecmp(command, "rm") == 0) return rm_;
	if(strcasecmp(command, "rmdir") == 0) return rmdir_;
	if(strcasecmp(command, "ls") == 0) return ls_;
	if(strcasecmp(command, "exit") == 0) return exit_;
	if(strcasecmp(command, "pause") == 0) return pause_;
	if(strcasecmp(command, "output") == 0) return output_;
	if(strcasecmp(command, "input") == 0) return input_;
	if(strcasecmp(command, "readall") == 0) return readall_;
	if(strcasecmp(command, "readline") == 0) return readline_;
	if(strcasecmp(command, "loadlib") == 0) return loadlib_;
	if(strcasecmp(command, "unloadlib") == 0) return unloadlib_;
	if(strcasecmp(command, "runlibfunc") == 0) return runlibfunc_;
	if(command[0] == '-') return comment_;
	if(command[0] == '#') return comment_;
	if(command[0] == ':' && command[1] != ':') return label_;
	if(command[0] == ':' && command[1] == ':') return comment_;
	return none_;
}

bool Neem::parseline(char *line) {
	while(isspace((uint8_t) *line)) line++; //Remove leading spaces
	if(*line == '\0') return 1; //if, after spaces, it's blank, just return
	
	char *params = splitstring(line, ' ');
	{ //Scope this since instruction will just be put into the vector
		types currenttype = gettype(line);
		if(currenttype == comment_) return true; //This is so we can /this/ properly
		else if(currenttype == none_) {
			std::string l = line;
			alert('!', "'%s' is not a command", NULL, &l);
			return false;
		} else if(currenttype != import_) { //Special command
			instruction i;
			i.type = currenttype;
			instructions.push_back(i);
		} else { //If import_
			if(params == NULL) {
				alert('!', "Can't import", NULL);
				return false;
			}
			if(!readfilebyline(params, parseline)) return false;
			return true;
		}
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case fi_:
			break;
		case echo_:
			last->value = (params != NULL) ? params : ""; //value to print (optional)
			last->func = command_echo;
			break;
		case exit_:
			last->func = command_exit;
			break;
		case setsystem_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			last->func = command_setsystem;
			break;
		case set_:
			last->extravalue = splitstring(params, '='); //value to set to
			last->value = params; //varname
			last->func = command_set;
			break;
		case prompt_: //type input
			last->extravalue = splitstring(params, '='); //Prompt
			last->value = params; //variable
			last->func = command_prompt;
			break;
		case sum_:
			last->extravalue = splitstring(params, '='); //Calculation
			last->value = params; //variable
			last->func = command_sum;
			break;
		case if_:
			last->extravalue = setifcheck(last, params); //sets the if function and returns after the op
			last->value = params; //setif \0s the left part
			last->func = command_if;
			break;
		case else_:
			last->func = command_else;
			break;
		case switch_:
			last->value = params;
			last->func = command_switch;
			break;
		case case_:
			last->value = (params != NULL) ? params : "";
			last->func = command_case;
			break;
		case for_:
			{
				char *arrstring = splitstring(params, ' ');
				char *delim = splitstring(arrstring, ' ');
				last->xxxtravalue = (delim != NULL) ? delim : std::string(1,FORCHARCHECK); //The delimiter
				last->extravalue = arrstring; //input/arr var
				last->value = params; //index value var
			}
			last->func = command_for;
			break;
		case rof_:
			last->func = command_rof;
			break;
		case goto_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = command_goto;
			break;
		case call_:
			last->value = (params[0] == ':') ? params+1 : params; //Remove : from label -> goto :label
			last->func = command_call;
			break;
		case inc_:
			{
				char *temp = splitstring(params, '=');
				if(temp != NULL) last->extravalue = temp; //Number to add (optional)
				else last->extravalue = "";
				last->value = params;
			}
			last->func = command_inc;
			break;
		case label_:
			last->value = line+1; //label name but with : removed
			break;
		case strftime_:
			last->extravalue = splitstring(params, '='); //strftime
			last->value = params; //varname
			last->func = command_strftime;
			break;
		case sleep_:
			last->value = params; //Time to sleep
			last->func = command_sleep;
			break;
		case start_:
			last->value = params; //Program name
			last->func = command_start;
			break;
		case pwd_:
			last->func = command_pwd;
			break;
		case cd_:
			if(params == NULL) last->extravalue = "";
				else last->value = params; //Directory name
			last->func = command_cd;
			break;
		case ls_:
			last->value = (params != NULL) ? params : ".";
			last->func = command_ls;
			break;
		case rm_:
			if(params == NULL) {alert('!', "Cannot have a blank rm statement"); return false;}
			last->value = params;
			last->func = command_rm;
			break;
		case rmdir_:
			if(params == NULL) {alert('!', "Cannot have a blank rmdir statement"); return false;}
			last->value = params;
			last->func = command_rmdir;
			break;
		case pause_:
			last->func = command_pause;
			break;
		case loadlib_:
			last->value = params; //libname
			last->func = command_loadlib;
			break;
		case unloadlib_:
			last->value = params; //lib
			last->func = command_unloadlib;
			break;
		case runlibfunc_:
			{ //scope this bit
				char *temp = splitstring(params, ' ');
				last->xxxtravalue = splitstring(temp, ' '); //args
				last->extravalue = temp; //function
				last->value = params; //library
			}
			last->func = command_runlibfunc;
			break;
		case output_:
			{
				char *temp = splitstring(params, ' ');
				last->value = params; //filename
				if(temp != NULL) last->extravalue = temp; //file attributes (optional)
				else last->extravalue = "a"; //default to append
			}
			last->func = command_output;
			break;
		case input_:
			last->value = params; //filename
			last->func = command_input;
			break;
		case readall_:
			last->value = params; //Variable name
			last->func = command_readall;
			break;
		case readline_:
			last->value = (params) ? params : ""; //Variable name (optional)
			last->func = command_readline;
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
