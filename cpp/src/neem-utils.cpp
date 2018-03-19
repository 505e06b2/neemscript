#include "neem.h"

std::string Neem::getstrftime(size_t size, const char *format) {
	struct tm *tm;
	char str_date[size];
	time_t t = time(NULL);
	tm = localtime(&t);
	strftime(str_date, sizeof(str_date), format, tm);
	return str_date;
}

std::string Neem::parsevarval(std::string *value) {
	return parsevariables(value->c_str());
}

std::string Neem::getcurrentdir() {
	char currentdir[256];
	getcwd(currentdir, sizeof(currentdir));
	return currentdir;
}

char *Neem::splitstring(char *str, const char token) {
	for(; *str; str++) {
		if(*str == token) {
			*str = '\0';
			return str+1;
		}
	}
	return NULL;
}