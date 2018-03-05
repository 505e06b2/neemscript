#include "neem.h"

#define MAX_LINE_LEN 255

void Neem::init() {
	instructions.clear();
	instructions.shrink_to_fit();
}

Neem::types Neem::gettype(char *command) {
	if(strcasecmp(command, "echo") == 0) return echo_;
	return none_;
}

void Neem::parseline(char *line) {
	char *params = line + strlen(strtok(line, " "))+1; //strtok splits, then strlen gets where the \0 is, then add that "index" to the pointer
	{ //Scope this since instruction is in the 
		instruction i;
		i.type = gettype(line);
		if(i.type == none_) return; //This is so we can /this/ properly
		instructions.push_back(i);
	}

	instruction *last = &instructions.back();
	switch(last->type) {
		case echo_:
			last->value = (char*)malloc(strlen(params)+1); strcpy(last->value, params); //Put it on one line because it's doing a single thing
			last->func = [](instruction *i, uint16_t index){printf("%s\n", i->value); return -1;};
			break;
	};
}

void Neem::interpret(char *fname) {
	init();
	
	FILE *file;
	char linebuffer[MAX_LINE_LEN];
	uint8_t length; //tied to MAX_LINE_LEN
	
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

Neem::~Neem() {
	//Clean the mallocs
	for(std::vector<instruction>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		free(it->value);
		free(it->extravalue);
	}
}