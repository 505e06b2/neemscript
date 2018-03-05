#ifndef __NEEM_H
#define __NEEM_H

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
		void interpret(char *text);
		~Neem();
	private:
		struct cmp_str { bool operator()(char const *a, char const *b) { return strcmp(a, b) < 0; } }; // Need this for map it won't work right
		enum types {none_, echo_, set_, goto_, call_, inc_, fi_, if_};
		typedef struct instruction {
			types type = none_;
			char *value = NULL;
			char *extravalue = NULL;
			std::function<int(struct instruction *i, uint16_t)> func = NULL;
		} instruction;
		FILE *outstream;
		std::vector<instruction> instructions;
		std::map<const char*, char*, cmp_str> variables;
		
		void parseline(char *);
		types gettype(char *);
		void cleanup();
};

#endif