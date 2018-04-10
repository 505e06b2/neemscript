#include "neem.h"

char *Neem::setifcheck(instruction *i, char *string) {
	while(*string) {
		switch(*string) {
			
			case '<':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(stold(left) <= stold(right)) return true; return false; };
					return (string+2);
				} else {
					i->check = [](std::string left, std::string right) { if(stold(left) < stold(right)) return true; return false; };
					return (string+1);
				}
				
			case '>':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(stof(left) >= stof(right)) return true; return false; };
					return (string+2);
				} else {
					i->check = [](std::string left, std::string right) { if(stof(left) > stof(right)) return true; return false; };
					return (string+1);
				}
				
			case '!':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(left != right) return true; return false; };
					return (string+2);
				} else { //Case insensitive !equals
					i->check = [](std::string left, std::string right) { if(strcasecmp(left.c_str(), right.c_str()) != 0) return true; return false; };
					return (string+1);
				}
				
			case '=':
				*string = '\0';
				if(*(string+1) == '=') {
					i->check = [](std::string left, std::string right) { if(left == right) return true; return false; };
					return (string+2);
				} else { //Case insensitive !equals
					i->check = [](std::string left, std::string right) { if(strcasecmp(left.c_str(), right.c_str()) == 0) return true; return false; };
					return (string+1);
				}
		}
		string++;
	}
	printf("[!] If string not correct: %s\n", string);
}