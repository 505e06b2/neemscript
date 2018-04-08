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
	uint8_t exclamations = 0;
	std::string out = parsevariables(value->c_str(), '%', &exclamations);
	if(exclamations >= 2) out = parsevariables(out.c_str(), '!', NULL);
	return out;
}

std::string Neem::getcurrentdir() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	return currentdir;
}

std::string Neem::listdir(const char *c, const char split) {
	std::string temp = "";
	struct dirent *d;
	DIR *dir = opendir(((c != NULL) ? c : "."));
	bool prepend = false;
	while((d = readdir(dir)) != NULL) {
		if(d->d_name[0] != '.') {
			if(prepend) temp += split;
			temp += d->d_name;
			prepend = true;
		}
	}
	closedir(dir);
	return temp;
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

Neem::parsedstrings *Neem::parseallstrings(parsedstrings *p, instruction *i) {
	if(i->value != "") p->value = parsevarval(&i->value);
	if(i->extravalue != "") p->extravalue = parsevarval(&i->extravalue);
	if(i->xxxtravalue != "") p->xxxtravalue = parsevarval(&i->xxxtravalue);
	return p;
}

int Neem::alert(const char type, const char *format, uint32_t *index, std::string *value, std::string *extravalue) {
	//This was going to be all fancy by only having 1 fprintf function but why not just use the buffers given to you?
	//It's an error, it's meant to alert you, not be efficient
	FILE *output = stderr;
	fprintf(output, "[!] ");
	if(index != NULL) {
		fprintf(output, "%u:", (*index)+1);
	}
	if(extravalue != NULL) {
		fprintf(output, format, value->c_str(), extravalue->c_str());
	} else if(value != NULL) {
		fprintf(output, format, value->c_str());
	} else {
		fprintf(output, format);
	}
	fprintf(output, "\n");
	return (type == '!') ? -2 : -1;
}