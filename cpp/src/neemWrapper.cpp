#include "neem.h"
#include "neemWrapper.h"

extern "C" {
        Neem* neem_new() {
                return new Neem();
        }

        void neem_interpret(Neem* v, char *text) {
                v->interpret(text);
        }

        void neem_del(Neem* v) {
                delete v;
        }
}