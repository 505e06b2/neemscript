#include "neem.h"

std::string Neem::dynamic_time(char *c) { return getstrftime(9, "%H:%M:%S"); }

std::string Neem::dynamic_date(char *c) { return getstrftime(9, "%d/%m/%y"); }

std::string Neem::dynamic_cd(char *c) { return getcurrentdir(); }

std::string Neem::dynamic_ls(char *c) { return listdir(c, FORCHARCHECK); }

std::string Neem::dynamic_upper(char *c) { return changecase(c, 'a', 'z', -32); }

std::string Neem::dynamic_lower(char *c) { return changecase(c, 'A', 'Z', +32); }

std::string Neem::dynamic_path(char *c) { return getenv("PATH"); }

std::string Neem::dynamic_scriptpath(char *c) { return scriptpath; }

std::string Neem::dynamic_sum(char *c) {
	char *format = splitstring(c, ';');
	double sum = dosum(c);
	if(format != NULL) {
		char buffer[20];
		char formatbuffer[8];
		sprintf(formatbuffer, "%%%slf", format);
		sprintf(buffer, formatbuffer, sum);
		return std::string(buffer);
	}
	return std::to_string(sum);
}

std::string Neem::dynamic_system(char *c) {
	std::string temp = c;
	const char *val = getenv(parsevarval(&temp).c_str());
	if(val == NULL) {
		return ""; //Blank is our NULL
	} else if(val[0] == '\0') {
		return " "; //To show it exists but is just blank
	} else {
		return val;
	} 
}

std::string Neem::dynamic_strftime(char *c) {
	char *temp = c;
	for(; *temp; temp++) if(*temp == '$') *temp = '%';
	return getstrftime(64, c);
}

std::string Neem::dynamic_epoch(char *c) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return std::to_string(((long long int)tp.tv_sec) * 1000000ll + 
                        (long long int)tp.tv_usec); 
}

std::string Neem::dynamic_librun(char *c) {
	if(c == NULL) return "[ERROR: FORMAT INCORRECT]";
	char *funcname = splitstring(c, ';');
	if(funcname == NULL) return "[ERROR: NO FUNCTION NAME SPECIFIED]";
	char *args = splitstring(funcname, ';');
	if(loadedlibs.find(c) == loadedlibs.end()) return "[ERROR: LIB NOT LOADED]";
	
	std::string libname = c;
	char *(*functionpointer)(const char *) = getlibraryfunction(&libname, funcname);
	if(functionpointer == NULL) return "[ERROR: CANNOT FIND FUNCTION IN LIB]";
	char *ret = functionpointer(args);
	if(ret != NULL) {
		std::string retval = ret;
		free(ret);
		return retval;
	}
	return "";
}

std::string Neem::dynamic_read(char *c) {
	if(c == NULL) return "[ERROR: NO FILE SPECIFIED]";
	FILE *f = fopen(c, "r");
	if(f == NULL) return "[ERROR: CAN'T OPEN FILE]";
	//Get size of file
	fseek (f, 0, SEEK_END);
	size_t s = ftell(f) + 1;
	rewind(f);
	char *buffer = (char *)calloc(s, sizeof(char));
	if(buffer == NULL) return "[ERROR: OUT OF MEMORY?]";
	fread(buffer, sizeof(char), s, f);
	std::string returnval = buffer;
	free(buffer);
	return returnval;
}

std::string Neem::dynamic_filename(char *c) {
	char *filename = c;
	for(; *c; c++) {
		if(*c == '/' || *c == '\\') filename = ++c;
	}
	std::string filenamestring = filename;
	return filenamestring;
}

std::string Neem::dynamic_pathname(char *c) {
	char *filename = c;
	char *path = c;
	for(; *c; c++) {
		if(*c == '/' || *c == '\\') filename = ++c;
	}
	*filename = '\0';
	std::string pathname = path;
	return pathname;
}

std::string Neem::dynamic_strlen(char *c) {
	return std::to_string(strlen(c));
}

std::string Neem::dynamic_arrlen(char *c) {
	if(*c == '\0') return "0";
	char defaultdelim = FORCHARCHECK;
	char *delim = splitstring(c, ';');
	if(delim == NULL) delim = &defaultdelim;
	uint32_t counts = 1;
	for(; *c; c++) if(*c == *delim) counts++;
	return std::to_string(counts);
}

std::string Neem::dynamic_ext(char *c) {
	char *ext = NULL;
	for(; *c; c++) {
		if(*c == '.') ext = ++c;
	}
	std::string extstring = (ext != NULL) ? ext : "";
	return extstring;
}

std::string Neem::dynamic_os(char *c) { 
		#ifdef _WIN32
			return "windows";
		#elif __linux__
			return "linux";
		#elif __unix__
			return "unix";
		#else
			return "unknown";
		#endif 
}

std::string Neem::dynamic_pointer(char *c) {
	char *end;
	const char *toint = (const char *)strtoull(c, &end, 0);
	if(toint == 0) {
		alert('#', "Pointer not valid");
		return "";
	}
	return toint;
}
