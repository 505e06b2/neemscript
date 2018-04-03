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
		enum types {none_, comment_, echo_, set_, get_, goto_, call_, inc_, dec_, epoch_, fi_, if_, sum_, label_, strftime_, sleep_, start_, pwd_, cd_, ls_, pause_, output_, loadlib_, runlibfunc_, unloadlib_};
		typedef struct instruction {
			types type = none_;
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
			std::function<int(struct instruction *i, uint32_t)> func = [](instruction *i, uint32_t index){return -1;}; //Needs to be this or it'll kill the program
			std::function<bool(std::string, std::string)> check = NULL;
		} instruction;
		
		std::vector<instruction> instructions;
		std::map<const std::string, std::string> variables;
		std::map<const std::string, std::function<std::string()>> globalvariables;
		uint16_t eof = -2; //to stop overflow
		FILE *outputhandle = stdout;
		
		bool parseline(char *, uint32_t);
		types gettype(char *);
		void cleanup();
		std::string parsevariables(const char *, const char, uint8_t *);
		char *setifcheck(instruction *, char *);
		char *splitstring(char *, const char); //strtok replacement
		float dosum(const char *);

		std::map<const std::string, void*> loadedlibs;
		bool loadlibrary(const char *, size_t);
		int runlibraryfunction(std::string *, const char *, const char *);
		void freelibrary(void *);
		
		//Utils
		std::string getstrftime(size_t, const char*);
		std::string parsevarval(std::string *); //wrapper for parsevariables
		std::string getcurrentdir();
};

#endif
