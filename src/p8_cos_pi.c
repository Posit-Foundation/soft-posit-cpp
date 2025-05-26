#include "internals.h"
#include "platform.h"
#include <math.h>

// posit8_t p8_cos_pi(posit8_t pA) {
//     union ui8_p8 uA;
//     double doubleA;
    
//     uA.p = pA;
    
//     // Check for special cases
//     if (uA.ui == 0x80) {
//         // If NaR, return NaR
//         return pA;
//     }
    
//     // Convert posit to double
//     doubleA = convertP8ToDouble(pA);
    
//     // Calculate cos(pi*x) ucosg standard math library
//     double result = cos(M_PI * doubleA);
    
//     // Convert back to posit
//     return convertDoubleToP8(result);
// } 

#include "p8_LUTs/p8_cos_pi_LUT.h"

posit8_t p8_cos_pi(posit8_t pA) {
    union ui8_p8 uA;
    uA.p = pA;

    // Check if input is NaR
    if (uA.ui == 128) {
    // Return NaR for NaR
    uA.ui = 0x80; // NaR value
    return uA.p;
    }
    
    // Look up the result from the table
    union ui8_p8 uZ;
    uZ.ui = p8cospi[uA.ui];
    return uZ.p;
}