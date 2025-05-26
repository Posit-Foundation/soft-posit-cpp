#include "internals.h"
#include "platform.h"
#include <math.h>

// posit8_t p8_exp2(posit8_t pA) {

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
    
//     // Calculate 2^x using standard math library
//     double result = pow(2.0, doubleA);
    
//     // Convert back to posit
//     return convertDoubleToP8(result);
// } 

#include"p8_LUTs/p8_exp2_LUT.h"

posit8_t p8_exp2(posit8_t pA) {
    union ui8_p8 uA;
    uA.p = pA;

    // Check if input is NaR
    if (uA.ui == 128) {
    // Return NaR for NaR
    uA.ui = 0x80; // NaR value
    return uA.p;
    }

    // Special case: if input is +0, return exact 1.0 (exp2(0) = 1)
    if (uA.ui == 0x00) {
        union ui8_p8 uOne;
        uOne.ui = 0x40; // Posit8 bit pattern for 1.0
        return uOne.p;
    }
    
    // Look up the result from the table
    union ui8_p8 uZ;
    uZ.ui = p8exp2[uA.ui];
    return uZ.p;
}