#include "neem.h"
#include "neemWrapper.h"

extern "C" {
        Neem* neem_new() {
                return new Neem();
        }

        void neem_interpretFile(Neem* v, char *text) {
                v->interpretFile(text);
        }

        void neem_del(Neem* v) {
                delete v;
        }
}