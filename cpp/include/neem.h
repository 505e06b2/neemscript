#ifndef __NEEM_H
#define __NEEM_H

#define MAX_LINE_LEN 1024
#define FORCHARCHECK '|'

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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
		void interpretFile(const char *);
		void interpretBuffer(const char *);
		void interpretLine(const char *);
		void setVariable(const char *, const char *);
		const char *getVariable(const char *);
		~Neem();
		Neem();
	private:
		void runInstructions();
		enum types {none_, comment_, echo_, set_, prompt_, setsystem_, import_, goto_, call_, inc_, fi_, if_, else_, for_, rof_, sum_, label_, strftime_, sleep_, start_, pwd_, cd_, ls_, pause_, output_, input_, readall_, readline_, loadlib_, runlibfunc_, unloadlib_};
		typedef struct instruction {
			types type = none_;
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
			std::function<int(struct instruction *i, uint32_t)> func = [](instruction *i, uint32_t index){return -1;}; //Needs to be this or it'll kill the program
			std::function<bool(std::string, std::string)> check = NULL;
		} instruction;
		
		typedef struct parsedstrings {
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
		} parsedstrings;
		parsedstrings *parseallstrings(parsedstrings *, instruction *);
		
		std::vector<instruction> instructions;
		std::map<const std::string, std::string> variables;
		std::map<const std::string, std::function<std::string(char *)>> dynamicvariables;
		uint16_t eof = -2; //to stop overflow
		FILE *outputhandle = stdout;
		FILE *inputhandle = NULL;
		
		bool parseline(char *);
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
		int setenvvar(std::string *, std::string *);
		
		//Utils
		std::string getstrftime(size_t, const char*);
		std::string parsevarval(std::string *); //wrapper for parsevariables
		std::string getcurrentdir();
		std::string listdir(const char *, const char);
		int alert(const char, const char *, uint32_t * = NULL, std::string * = NULL, std::string * = NULL);
		int searchfortag(uint32_t *, const types, const types);
		bool readfilebyline(const char *, std::function<bool(char *)>);
};

#endif
