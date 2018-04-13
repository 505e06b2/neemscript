#include "neem.h"

std::string Neem::dynamic_time(char *c) { return getstrftime(9, "%H:%M:%S"); }

std::string Neem::dynamic_date(char *c) { return getstrftime(9, "%d/%m/%y"); }

std::string Neem::dynamic_cd(char *c) { return getcurrentdir(); }

std::string Neem::dynamic_ls(char *c) { return listdir(c, FORCHARCHECK); }

std::string Neem::dynamic_upper(char *c) { return changecase(c, 'a', 'z', -32); }

std::string Neem::dynamic_lower(char *c) { return changecase(c, 'A', 'Z', +32); }

std::string Neem::dynamic_path(char *c) { return getenv("PATH"); }

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
	using namespace std::chrono;
	auto casted = time_point_cast<microseconds>(system_clock::now()).time_since_epoch();
	return std::to_string(casted.count()); 
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
