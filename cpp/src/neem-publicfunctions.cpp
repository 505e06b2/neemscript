#include "neem.h"

Neem::~Neem() {
	cleanup();
}

//Variable stuff
Neem::Neem() { //Set up globals
	dynamicvariables["TIME"] = [this](char *c = NULL) {
		return getstrftime(9, "%H:%M:%S");
	};
	
	dynamicvariables["DATE"] = [this](char *c = NULL) {
		return getstrftime(9, "%d/%m/%y");
	};
	
	dynamicvariables["CD"] = [this](char *c = NULL) {
		return getcurrentdir();
	};
	
	dynamicvariables["LS"] = [this](char *c = NULL) {
		return listdir(c, FORCHARCHECK);
	};
	
	dynamicvariables["OS"] = [this](char *c = NULL) {
		#ifdef _WIN32
			return "windows";
		#elif __linux__
			return "linux";
		#elif __unix__
			return "unix";
		#else
			return "unknown";
		#endif
	};
	
	dynamicvariables["PATH"] = [this](char *c = NULL) {
		return getenv("PATH");
	};
	
	dynamicvariables["EPOCH"] = [this](char *c = NULL) {
		using namespace std::chrono;
		auto casted = time_point_cast<microseconds>(system_clock::now()).time_since_epoch();
		return std::to_string(casted.count());
	};
	
	dynamicvariables["STRFTIME"] = [this](char *c = NULL) {
		char *temp = c;
		for(; *temp; temp++) if(*temp == '$') *temp = '%';
		return getstrftime(64, c);
	};
	
	dynamicvariables["SYSTEM"] = [this](char *c = NULL) {
		std::string temp = c;
		const char *val = getenv(parsevarval(&temp).c_str());
		if(val == NULL) {
			return ""; //Blank is our NULL
		} else if(val[0] == '\0') {
			return " "; //To show it exists but is just blank
		} else {
			return val;
		}
	};
	
	dynamicvariables["POINTER"] = [this](char *c = NULL) {
		char *end;
		const char *toint = (const char *)strtoull(c, &end, 0);
		if(toint == 0) {
			alert('#', "Pointer not valid");
			return "";
		}
		return toint;
	};
}

void Neem::setVariable(const char *name, const char *value) {
	variables[name] = value;
}

const char *Neem::getVariable(const char *name) {
	std::map<const std::string, std::string>::iterator variableinter;
	if((variableinter = variables.find(name)) != variables.end()) return variableinter->second.c_str();
	return NULL;
}

//Interpreting
void Neem::interpretFile(char *fname) {
	{ //scope all this since we don't need it after
		FILE *file;
		char linebuffer[MAX_LINE_LEN];
		uint16_t length; //tied to MAX_LINE_LEN
	
		if((file = fopen(fname, "r")) == NULL) {
			std::string f = fname;
			alert('!', "Can't open '%s'", NULL, &f);
			return;
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
			if(!parseline(linebuffer)) return;
		}
	
		fclose(file);
	}
	
	runInstructions(); //The meat of the program
}

void Neem::interpretBuffer(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '\n') {
				currentline.push_back('\0');
				if(!parseline(currentline.data())) return;
				std::vector<char>().swap(currentline); //Clean the vector
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data())) return;
	}
	
	runInstructions();
}

void Neem::interpretLine(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '&' && *(buffer+1) == '&') {
				currentline.push_back('\0');
				if(!parseline(currentline.data())) return;
				std::vector<char>().swap(currentline); //Clean the vector
				buffer++;
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data())) return;
	}
	
	runInstructions();
}