#include "neem.h"

#ifdef _WIN32

	#include <windows.h>
	
	bool Neem::loadlibrary(const char *fname) {
		void *lib = LoadLibrary(fname);
		if(lib == NULL) return false;
		loadedlibs[fname] = lib;
		return true;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		int(*funcpointer)(const void *) = (int(*)(const void *))GetProcAddress((HMODULE)loadedlibs[*libname], name);
		return funcpointer(args);
	}
	
//#elif __linux__
#else
	
	bool Neem::loadlibrary(const char *fname) {
		return false;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		return -2;
	}
	
#endif