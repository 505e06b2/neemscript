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
	return parsevariables(parsebuffer, value->c_str());
}
