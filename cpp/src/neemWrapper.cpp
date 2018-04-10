#include "neem.h"
#include "neemWrapper.h"

#ifdef __cplusplus
extern "C" {  
#endif

Neem* neem_new() {
	return new Neem();
}

void neem_interpretFile(Neem* v, char *text) {
	v->interpretFile(text);
}

void neem_interpretBuffer(Neem* v, const char *text) {
	v->interpretBuffer(text);
}

void neem_interpretLine(Neem* v, const char *text) {
	v->interpretLine(text);
}

void neem_setVariable(Neem* v, const char *name, const char *value) {
	v->setVariable(name, value);
}

const char *neem_getVariable(Neem* v, const char *name) {
	return v->getVariable(name);
}

void neem_del(Neem* v) {
	delete v;
}

#ifdef __cplusplus
}
#endif