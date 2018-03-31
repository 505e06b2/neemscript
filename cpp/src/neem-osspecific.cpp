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
		auto it = loadedlibs.find(*libname);
		if(it == loadedlibs.end()) return -27201;
		int(*funcpointer)(const void *) = (int(*)(const void *))GetProcAddress((HMODULE)loadedlibs[*libname], name);
		if(funcpointer == NULL) return -27202; //it's just obscure
		return funcpointer(args);
	}
	
	void Neem::freelibrary(void *lib) {
		FreeLibrary((HMODULE)lib);
	}
	
//#elif __linux__
#else
	
	bool Neem::loadlibrary(const char *fname) {
		return false;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		return -2;
	}
	
	void Neem::freelibrary(void *lib) {
		return;
	}
	
#endif