#ifndef __NEEMWRAPPER_H
#define __NEEMWRAPPER_H

//https://www.teddy.ch/c++_library_in_c/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Neem Neem;

Neem* neem_new();

void neem_interpretFile(Neem* v, char *text); //All public functions need to be wrapped for C
void neem_interpretBuffer(Neem* v, const char *text);

void neem_del(Neem* v);

#ifdef __cplusplus
}
#endif
#endif