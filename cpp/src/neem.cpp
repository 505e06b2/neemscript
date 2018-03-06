#include "neem.h"

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	if(strcasecmp(command, "set") == 0) return set_;
	return none_;
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
			last->value = (char*)malloc(strlen(params)+1); strcpy(last->value, params); //Put it on one line because it's doing a single thing
			last->func = [](instruction *i, uint16_t index){ printf("%s\n", i->value); return -1; };
			break;
		case set_:
			last->value = (char*)malloc(strlen(params)+1); strcpy(last->value, params);
			last->extravalue = last->value + strlen(strtok(last->value, "="))+1; //So the strtok returns a pointer to the variable; strlen gets where the \0 is, then +1 for the start of the next, then add this "index" to the char pointer
			last->func = [this](instruction *i, uint16_t index){ variables[i->value] = i->extravalue; return -1; };
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
		
		if(linebuffer[length-2] == '\r') linebuffer[length-2] = '\0'; //did have a loop but this seems like it would be faster and neater?
		else if(linebuffer[length-1] == '\n') linebuffer[length-1] = '\0'; //Can line endings be different? Is it worth checking for all \r or \n??
		
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
	//Clean the mallocs
	for(std::vector<instruction>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		free(it->value);
		free(it->extravalue);
	}
	variables.clear();
	instructions.clear();
	instructions.shrink_to_fit();
}

Neem::~Neem() {
	cleanup();
}
