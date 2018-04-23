#include "neem.h"

Neem::~Neem() {
	cleanup();
}

Neem::Neem() { //Set up dynamic vars
	dynamicvariables["TIME"] = &Neem::dynamic_time;
	dynamicvariables["DATE"] = &Neem::dynamic_date;
	dynamicvariables["CD"] = &Neem::dynamic_cd;
	dynamicvariables["LS"] = &Neem::dynamic_ls;
	dynamicvariables["OS"] = &Neem::dynamic_os;
	dynamicvariables["UPPER"] = &Neem::dynamic_upper;
	dynamicvariables["LOWER"] = &Neem::dynamic_lower;
	dynamicvariables["PATH"] = &Neem::dynamic_path;
	dynamicvariables["EPOCH"] = &Neem::dynamic_epoch;
	dynamicvariables["STRFTIME"] = &Neem::dynamic_strftime;
	dynamicvariables["SYSTEM"] = &Neem::dynamic_system;
	dynamicvariables["POINTER"] = &Neem::dynamic_pointer;
	dynamicvariables["SUM"] = &Neem::dynamic_sum;
	dynamicvariables["LIBRUN"] = &Neem::dynamic_librun;
	dynamicvariables["FILENAME"] = &Neem::dynamic_filename;
	dynamicvariables["PATHNAME"] = &Neem::dynamic_pathname;
	dynamicvariables["SCRIPTPATH"] = &Neem::dynamic_scriptpath;
	dynamicvariables["READ"] = &Neem::dynamic_read;
	dynamicvariables["STRLEN"] = &Neem::dynamic_strlen;
	dynamicvariables["ARRLEN"] = &Neem::dynamic_arrlen;
	dynamicvariables["EXT"] = &Neem::dynamic_ext;
	dynamicvariables["REPLACE"] = &Neem::dynamic_replace;
	dynamicvariables["REPLACEALL"] = &Neem::dynamic_replaceall;
	
	std::map<const std::string, std::string> firstscope;
	variablesinscopes.push_back(firstscope);
}

void Neem::setVariable(const char *name, const char *value) {
	std::string n = name;
	std::string v = value;
	setvariabletoscope(name, value);
}

const char *Neem::getVariable(const char *name) {
	std::string in = name;
	std::string ret = getvariablefromscope(in);
	if(ret != "") return ret.c_str();
	return NULL;
}

//Interpreting
void Neem::interpretFile(const char *fname) {
	//Set scriptpath
	if(fname[0] == '/' || ((fname[1] == ':') && (fname[2] == '/' || fname[2] == '\\'))) { //if the file passed in is absolute: /file or C:\file or C:/file
		scriptpath = fname;
	} else {
		char cwdbuffer[MAX_LINE_LEN + 512]; //for both the cwd, then the path passed in
		if(getcwd(cwdbuffer, MAX_LINE_LEN)) {
			#ifdef _WIN32
				strcat(cwdbuffer, "\\");
			#else
				strcat(cwdbuffer, "/");
			#endif
			strcat(cwdbuffer, fname);
			scriptpath = cwdbuffer;
		} else scriptpath = "BUFFER TOO SMALL, please look at function 'interpretFile' in publicfunctions to increase; maybe create an issue, if it happens a lot";
	}
	
	if(!readfilebyline(fname, &Neem::parseline)) return;
	runInstructions(); //The meat of the program
}

void Neem::interpretBuffer(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		uint32_t index = 0;
		for(; *buffer; buffer++) {
			if(*buffer == '\n') {
				currentline.push_back('\0');
				if(!parseline(currentline.data(), index++)) return;
				std::vector<char>().swap(currentline); //Clean the vector
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data(), index)) return;
	}
	
	runInstructions();
}

void Neem::interpretLine(const char *buffer) {
	{ //scope removes vector
		std::vector<char> currentline;
		for(; *buffer; buffer++) {
			if(*buffer == '&' && *(buffer+1) == '&') {
				currentline.push_back('\0');
				if(!parseline(currentline.data(), 0)) return;
				std::vector<char>().swap(currentline); //Clean the vector
				buffer++;
			} else {
				currentline.push_back(*buffer);
			}
		}
		//One more at the end
		currentline.push_back('\0');
		if(!parseline(currentline.data(), 0)) return;
	}
	
	runInstructions();
}