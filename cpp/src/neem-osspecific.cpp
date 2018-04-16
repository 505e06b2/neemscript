#include "neem.h"

#ifdef _WIN32

	#include <windows.h>
	
	bool Neem::loadlibrary(const char *path, size_t pathlen) {
		void *lib = LoadLibrary(path);
		if(lib == NULL) return false;
		loadedlibs[ filenamefrompath(path, pathlen) ] = lib;
		return true;
	}
	
	Neem::externalfunc Neem::getlibraryfunction(std::string *libname, const char *name) {
		return (externalfunc)GetProcAddress((HMODULE)loadedlibs[*libname], name);
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
	
	bool Neem::loadlibrary(const char *path, size_t pathlen) { // the line given would be: /usr/lib/test
		const char *filename = filenamefrompath(path, pathlen);
		char newpath[pathlen+3+3]; //+3 for "lib", +3 for ".so\0"
		
		uint16_t prefixlen = (uint16_t)(filename - path); //Take the pointers away to get the size
		uint16_t filenamelen = (uint16_t)(pathlen - prefixlen); // pathlen is the full path passed, while prefixlen is the /usr/lib/ bit
		
		memcpy(newpath, path, prefixlen); // = /usr/lib/
		memcpy(newpath+prefixlen, "lib", 3); // = /usr/lib/lib
		memcpy(newpath+prefixlen+3, filename, filenamelen); // = /usr/lib/libtest
		memcpy(newpath+prefixlen+3+filenamelen, ".so", 4); // = /usr/lib/libtest.so
		void *lib = dlopen(newpath, RTLD_LAZY);
		if(lib == NULL) return false;
		loadedlibs[filename] = lib;
		return true;
	}
	
	Neem::externalfunc Neem::getlibraryfunction(std::string *libname, const char *name) {
		return (externalfunc)dlsym(loadedlibs[*libname], name);
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