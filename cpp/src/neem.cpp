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
	instruction i;
	switch(i.type = gettype(line)) {
		case echo_:
			i.value = (char*)malloc(strlen(params)+1); strcpy(i.value, params); //Put it on one line because it's doing a single thing
			break;
	};
	instructions.push_back(i);
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
}

Neem::~Neem() {
	//Clean the mallocs
	for(std::vector<instruction>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		free(it->value);
		free(it->extravalue);
	}
}