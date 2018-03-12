#ifndef __NEEM_H
#define __NEEM_H

#define MAX_LINE_LEN 512
#define PARSE_BUFFER_LEN 256
#define MAX_VARNAME_LEN 64

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <vector>
#include <map>
#include <functional>
#include <string>

/*
//
//			 REALLY NEED TO MAKE A MORE CONVENIENT STRTOK / FIND ONE IF IT EXISTS
//
*/

class Neem {
	public:
		void interpretFile(char *text);
		~Neem();
		Neem();
	private:
		enum types {none_, echo_, set_, goto_, call_, inc_, fi_, if_, label_};
		typedef struct instruction {
			types type = none_;
			std::string value;
			std::string extravalue;
			std::function<int(struct instruction *i, uint16_t)> func = [](instruction *i, uint16_t index){return -1;}; //Needs to be this or it'll kill the program
			std::function<bool(std::string, std::string)> check = NULL;
		} instruction;
		FILE *outstream;
		std::vector<instruction> instructions;
		std::map<const std::string, std::string> variables;
		std::map<const std::string, std::function<std::string()>> globalvariables;
		uint16_t eof = -2; //to stop overflow
		//These next 2 are for when I start ifs; I need 2 buffers simultaneously
		char parsebuffer[PARSE_BUFFER_LEN]; //for ->value
		char parseextrabuffer[PARSE_BUFFER_LEN]; //for ->extravalue
		
		void parseline(char *);
		types gettype(char *);
		void cleanup();
		std::string parsevariables(char *, const char *);
		std::string parsevarval(std::string *); //wrapper for parsevariables
		char *setifcheck(instruction *, char *);
};

#endif
