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
#include <time.h>
#include <sys/time.h>

#include <vector>
#include <map>
#include <string>

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
		enum types {none_, comment_, echo_, set_, prompt_, setsystem_, setglobal_, import_, goto_, call_, inc_, fi_, if_, else_, switch_, case_, for_, rof_,
					exit_, label_, sleep_, start_, pwd_, cd_, rm_, rmdir_, ls_, pause_, output_, input_, readall_, readline_, setlocal_, endlocal_,
					libload_, librun_, libunload_};
					
		typedef char *(*externalfunc)(const char *);
					
		typedef struct instruction {
			types type = none_;
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
			int (Neem::*func)(struct instruction *i, uint32_t);
			bool (*check)(std::string, std::string);
		} instruction;
		
		typedef struct parsedstrings {
			std::string value;
			std::string extravalue;
			std::string xxxtravalue;
		} parsedstrings;
		
		typedef struct typeandfunc {
			types type;
			int (Neem::*func)(instruction *i, uint32_t);
		} typeandfunc;
		
		std::vector<instruction> instructions;
		std::vector<std::map<const std::string, std::string>> variablesinscopes;
		std::map<const std::string, std::string (Neem::*)(char *)> dynamicvariables;
		uint16_t eof = -2; //to stop overflow, I want it to be max, but it +1s and that makes things not work...
		FILE *outputhandle = stdout;
		FILE *inputhandle = NULL;
		std::string switchcheckstring = "";
		std::map<const std::string, void*> loadedlibs;
		std::string scriptpath = "";
		
		//neem.cpp
		bool parseline(char *, uint32_t);
		typeandfunc gettype(char *);
		void cleanup();
		void runInstructions();
		
		//special funcs
		std::string parsevariables(const char *, const char, uint8_t *); //parsevars.cpp
		char *setifcheck(instruction *, char *); //ifs.cpp
		float dosum(const char *); //sums.cpp

		//OS-Specific
		bool loadlibrary(const char *, size_t);
		externalfunc getlibraryfunction(std::string *, const char *);
		void freelibrary(void *);
		int setenvvar(std::string *, std::string *);
		void threadsleep(uint32_t);
		
		//Utils
		std::string getstrftime(size_t, const char*);
		std::string parsevarval(std::string *); //wrapper for parsevariables
		parsedstrings *parseallstrings(parsedstrings *, instruction *);
		std::string getcurrentdir();
		std::string listdir(const char *, const char);
		bool removedir(const char *);
		int alert(const char, const char *, uint32_t * = NULL, std::string * = NULL, std::string * = NULL);
		int searchfortag(uint32_t *, const types, const types);
		bool readfilebyline(const char *, bool(Neem::*)(char *, uint32_t)); //laod file and put contents in instructions vector
		const char *changecase(char *, const char, const char, int8_t);
		const char *filenamefrompath(const char *, size_t);
		char *splitstring(char *, const char); //strtok replacement
		std::string getvariablefromscope(std::string);
		void setvariabletoscope(std::string, std::string);
		
		//dynamic vars - dynamic.cpp
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
		std::string dynamic_sum(char *);
		std::string dynamic_librun(char *);
		std::string dynamic_filename(char *);
		std::string dynamic_pathname(char *);
		std::string dynamic_scriptpath(char *);
		std::string dynamic_read(char *);
		std::string dynamic_strlen(char *);
		std::string dynamic_ext(char *);
		
		//commands - commands/[command].cpp
		int command_none(instruction *, uint32_t);
		int command_echo(instruction *, uint32_t);
		int command_exit(instruction *, uint32_t);
		int command_setsystem(instruction *, uint32_t);
		int command_set(instruction *, uint32_t);
		int command_prompt(instruction *, uint32_t);
		int command_if(instruction *, uint32_t);
		int command_else(instruction *, uint32_t);
		int command_switch(instruction *, uint32_t);
		int command_case(instruction *, uint32_t);
		int command_for(instruction *, uint32_t);
		int command_rof(instruction *, uint32_t);
		int command_goto(instruction *, uint32_t);
		int command_call(instruction *, uint32_t);
		int command_inc(instruction *, uint32_t);
		int command_sleep(instruction *, uint32_t);
		int command_start(instruction *, uint32_t);
		int command_pwd(instruction *, uint32_t);
		int command_cd(instruction *, uint32_t);
		int command_ls(instruction *, uint32_t);
		int command_rm(instruction *, uint32_t);
		int command_rmdir(instruction *, uint32_t);
		int command_pause(instruction *, uint32_t);
		int command_libload(instruction *, uint32_t);
		int command_libunload(instruction *, uint32_t);
		int command_librun(instruction *, uint32_t);
		int command_output(instruction *, uint32_t);
		int command_input(instruction *, uint32_t);
		int command_readline(instruction *, uint32_t);
		int command_setlocal(instruction *, uint32_t);
		int command_endlocal(instruction *, uint32_t);
		int command_setglobal(instruction *, uint32_t);
		
};

#endif
