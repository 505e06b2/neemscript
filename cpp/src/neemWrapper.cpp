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

void neem_del(Neem* v) {
	delete v;
}

#ifdef __cplusplus
}
#endif