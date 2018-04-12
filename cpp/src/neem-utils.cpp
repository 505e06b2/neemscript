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
	if(getcwd(currentdir, sizeof(currentdir)) == NULL) return "";
	return currentdir;
}

int Neem::searchfortag(uint32_t *index, const types search, const types avoid) {
	uint8_t avoids = 1;
	for(int32_t e = instructions.size(), a = (*index)+1; a < e; a++) {
		if(instructions[a].type == search) {
			if(--avoids == 0) return a;
		} else if(instructions[a].type == avoid) {
			avoids++;
		}
	}
	return -1;
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

bool Neem::removedir(const char *fname) {
	DIR* directory;
	struct dirent* d;
	struct stat s_buf;
	char p_buf[MAX_LINE_LEN];

    directory = opendir(fname);
	if(directory == NULL) return false;

    while ((d = readdir(directory)) != NULL) {
		if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        sprintf(p_buf, "%s/%s", fname, d->d_name);
		if(stat(p_buf, &s_buf) != 0) return false;
        if(S_ISDIR(s_buf.st_mode)) {
			removedir(p_buf);
        } else {
			unlink(p_buf);
		}
    }

    closedir(directory);
    rmdir(fname);
	return true;
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

const char *Neem::changecase(char *c, const char a, const char z, int8_t alter) {
	char *r = c;
	for(; *c; c++) {
		if(*c >= a && *c <= z) *c += alter;
	}
	return r;
}

bool Neem::readfilebyline(const char *filename, std::function<bool(char *)> func) {
	FILE *file;
	char linebuffer[MAX_LINE_LEN];
	uint16_t length; //tied to MAX_LINE_LEN
	
	if((file = fopen(filename, "r")) == NULL) {
		std::string f = filename;
		alert('!', "Can't open '%s'", NULL, &f);
		return false;
	}
	
	for(uint32_t index = 0; fgets(linebuffer, sizeof(linebuffer), file); index++) {
		length = strlen(linebuffer);
		for(int i = length, e = length-3; i > e && i >= 0; i--) {
			switch(linebuffer[i]) { // 'remove' chars we really don't want
				case '\r':
				case '\n':
					linebuffer[i] = '\0';
			}
		}
		if(!func(linebuffer)) return false;
	}
	
	fclose(file);
	return true;
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