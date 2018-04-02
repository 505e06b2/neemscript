#include "neem.h"

#ifdef _WIN32

	#include <windows.h>
	
	bool Neem::loadlibrary(const char *fname, size_t fnamelen) {
		void *lib = LoadLibrary(fname);
		if(lib == NULL) return false;
		loadedlibs[fname] = lib;
		return true;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		int(*funcpointer)(const void *) = (int(*)(const void *))GetProcAddress((HMODULE)loadedlibs[*libname], name);
		if(funcpointer == NULL) return -27202; //it's just obscure
		return funcpointer(args);
	}
	
	void Neem::freelibrary(void *lib) {
		FreeLibrary((HMODULE)lib);
	}
	
#elif __unix__
	
	#include <dlfcn.h>
	
	bool Neem::loadlibrary(const char *fname, size_t fnamelen) {
		char fnamebuffer[fnamelen+3];
		memcpy(fnamebuffer, fname, fnamelen);
		memcpy(fnamebuffer+fnamelen, ".so", 4);
		printf(">> %s\n", fnamebuffer);
		void *lib = dlopen(fnamebuffer, RTLD_LAZY);
		if(lib == NULL) return false;
		loadedlibs[fname] = lib;
		return true;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		int(*funcpointer)(const void *) = (int(*)(const void *))dlsym(loadedlibs[*libname], name);
		if(funcpointer == NULL) return -27202; //it's just obscure
		return funcpointer(args);
	}
	
	void Neem::freelibrary(void *lib) {
		dlclose(lib);
	}
	
#else
	
	bool Neem::loadlibrary(const char *fname, size_t fnamelen) {
		return false;
	}
	
	int Neem::runlibraryfunction(std::string *libname, const char *name, const char *args) {
		return -2;
	}
	
	void Neem::freelibrary(void *lib) {
		return;
	}
	
#endif