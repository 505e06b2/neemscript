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
#include <vector>
#include <map>
#include <functional>

class Neem {
	public:
		void interpretFile(char *text);
		~Neem();
	private:
		struct cmp_str { bool operator()(char const *a, char const *b) { return strcmp(a, b) < 0; } }; // Need this for map it won't work right
		enum types {none_, echo_, set_, goto_, call_, inc_, fi_, if_};
		typedef struct instruction {
			types type = none_;
			char *value = NULL;
			uint16_t valuelen = 0; //store it since it'll be used a lot but never change
			char *extravalue = NULL;
			uint16_t extravaluelen = 0;
			std::function<int(struct instruction *i, uint16_t)> func = NULL;
		} instruction;
		FILE *outstream;
		std::vector<instruction> instructions;
		std::map<const char*, char*, cmp_str> variables; //cmpstr for the c string because c++ is annoying...
		uint16_t eof;
		//These next 2 are for when I start ifs; I need 2 buffers simultaneously
		char parsebuffer[PARSE_BUFFER_LEN]; //for ->value
		char parseextrabuffer[PARSE_BUFFER_LEN]; //for ->extravalue
		
		void parseline(char *);
		types gettype(char *);
		void cleanup();
		char *parsecommands(char *, char *, uint16_t);
};

#endif
