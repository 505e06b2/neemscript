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
#include <unistd.h>
#include <dirent.h>

#include <vector>
#include <map>
#include <functional> //function as vars
#include <string>
#include <chrono>//Time stuff
#include <thread>

class Neem {
	public:
		void interpretFile(char *text);
		~Neem();
		Neem();
	private:
		enum types {none_, comment_, echo_, set_, goto_, call_, inc_, fi_, if_, label_, strftime_, sleep_, start_, pwd_, cd_, ls_, pause_};
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
		
		bool parseline(char *);
		types gettype(char *);
		void cleanup();
		std::string parsevariables(char *, const char *);
		char *setifcheck(instruction *, char *);
		
		//Utils
		std::string getstrftime(size_t, const char*);
		std::string parsevarval(std::string *); //wrapper for parsevariables
		std::string getcurrentdir();
};

#endif
