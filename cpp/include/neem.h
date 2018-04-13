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
#include <sys/stat.h>

#include <vector>
#include <map>
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
		enum types {none_, comment_, echo_, set_, prompt_, setsystem_, import_, goto_, call_, inc_, fi_, if_, else_, switch_, case_, for_, rof_, sum_, exit_, label_, strftime_, sleep_, start_, pwd_, cd_, rm_, rmdir_, ls_, pause_, output_, input_, readall_, readline_, loadlib_, runlibfunc_, unloadlib_};
		typedef struct instruction {
			types type = none_;
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
			int (Neem::*func)(struct instruction *i, uint32_t) = command_none; //Needs to be this or it'll kill the program
			bool (*check)(std::string, std::string) = NULL;
		} instruction;
		
		typedef struct parsedstrings {
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
		} parsedstrings;
		parsedstrings *parseallstrings(parsedstrings *, instruction *);
		
		std::vector<instruction> instructions;
		std::map<const std::string, std::string> variables;
		std::map<const std::string, std::string (Neem::*)(char *)> dynamicvariables;
		uint16_t eof = -2; //to stop overflow, I want it to be max, but it +1s
		FILE *outputhandle = stdout;
		FILE *inputhandle = NULL;
		std::string switchcheckstring = "";
		
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
		bool removedir(const char *);
		int alert(const char, const char *, uint32_t * = NULL, std::string * = NULL, std::string * = NULL);
		int searchfortag(uint32_t *, const types, const types);
		bool readfilebyline(const char *, bool(Neem::*)(char *)); //laod file and put contents in instructions vector
		const char *changecase(char *, const char, const char, int8_t);
		
		//dynamic vars
		std::string dynamic_time(char *);
		std::string dynamic_date(char *);
		std::string dynamic_cd(char *);
		std::string dynamic_ls(char *);
		std::string dynamic_os(char *);
		std::string dynamic_upper(char *);
		std::string dynamic_lower(char *);
		std::string dynamic_path(char *);
		std::string dynamic_epoch(char *);
		std::string dynamic_strftime(char *);
		std::string dynamic_system(char *);
		std::string dynamic_pointer(char *);
		
		//commands
		int command_none(instruction *, uint32_t);
		int command_echo(instruction *, uint32_t);
		int command_exit(instruction *, uint32_t);
		int command_setsystem(instruction *, uint32_t);
		int command_set(instruction *, uint32_t);
		int command_prompt(instruction *, uint32_t);
		int command_sum(instruction *, uint32_t);
		int command_if(instruction *, uint32_t);
		int command_else(instruction *, uint32_t);
		int command_switch(instruction *, uint32_t);
		int command_case(instruction *, uint32_t);
		int command_for(instruction *, uint32_t);
		int command_rof(instruction *, uint32_t);
		int command_goto(instruction *, uint32_t);
		int command_call(instruction *, uint32_t);
		int command_inc(instruction *, uint32_t);
		int command_strftime(instruction *, uint32_t);
		int command_sleep(instruction *, uint32_t);
		int command_start(instruction *, uint32_t);
		int command_pwd(instruction *, uint32_t);
		int command_cd(instruction *, uint32_t);
		int command_ls(instruction *, uint32_t);
		int command_rm(instruction *, uint32_t);
		int command_rmdir(instruction *, uint32_t);
		int command_pause(instruction *, uint32_t);
		int command_loadlib(instruction *, uint32_t);
		int command_unloadlib(instruction *, uint32_t);
		int command_runlibfunc(instruction *, uint32_t);
		int command_output(instruction *, uint32_t);
		int command_input(instruction *, uint32_t);
		int command_readall(instruction *, uint32_t);
		int command_readline(instruction *, uint32_t);
		
};

#endif
