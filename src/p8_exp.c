#include "internals.h"
#include "platform.h"
#include "p8_LUTs/p8_exp_LUT.h"

posit8_t p8_exp(posit8_t pA) {
    union ui8_p8 uA;
    uint_fast8_t uiA;

    uA.p = pA;
    uiA = uA.ui;
    
    // Look up the result from the table
    uA.ui = p8Exp[uiA];
    
    return uA.p;
}
