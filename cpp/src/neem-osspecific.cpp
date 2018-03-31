#include "neem.h"

#ifdef _WIN32

	#include <windows.h>
	
	bool Neem::loadlibrary(const char *fname) {
		void *lib = LoadLibrary(fname);
		if(lib == NULL) return false;
		loadedlibs[fname] = lib;
		return true;
	}
	
//#elif __linux__
#else
	
	bool Neem::loadlibrary(const char *fname) {
		return false;
	}
	
#endif