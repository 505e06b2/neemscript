#ifndef __NEEM_H
#define __NEEM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <vector>

class Neem {
	public:
		void interpret(char *text);
		~Neem();
	private:
		enum types {none_, echo_, set_, goto_, call_, inc_, fi_, if_};
		typedef struct {
			types type = none_;
			char *value = NULL;
			char *extravalue = NULL;
			int8_t (*func)();
		} instruction;
		FILE *outstream;
		std::vector<instruction> instructions;
		
		void init();
		void parseline(char *line);
		types gettype(char *command);
};

#endif