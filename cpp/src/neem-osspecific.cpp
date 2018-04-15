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
	
	int Neem::setenvvar(std::string *name, std::string *value) {
		std::string format = *name + '=' + *value;
		if(_putenv(format.c_str()) != 0) return -2;
		return -1;
	}
	
	void Neem::threadsleep(uint32_t delay) {
		Sleep(delay);
	}
	
#elif __unix__
	
	#include <dlfcn.h>
	
	bool Neem::loadlibrary(const char *fname, size_t fnamelen) {
		char fnamebuffer[fnamelen+3+3];
		memcpy(fnamebuffer, "lib", 3);
		memcpy(fnamebuffer+3, fname, fnamelen);
		memcpy(fnamebuffer+fnamelen+3, ".so", 4);
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
	
	int Neem::setenvvar(std::string *name, std::string *value) {
		if(setenv(name->c_str(), value->c_str(), 1) != 0) return -2;
		return -1;
	}
	
	void Neem::threadsleep(uint32_t delay) {
		usleep(delay*1000);
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
	
	int Neem::setenvvar(const char *name, const char *value) {
		return -2;
	}
	
#endif