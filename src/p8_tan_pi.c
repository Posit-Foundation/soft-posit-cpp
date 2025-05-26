#include "internals.h"
#include "platform.h"
#include <math.h>

// posit8_t p8_tan_pi(posit8_t pA) {

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
    
//     // Handle special cases for tan
//     double modValue = fmod(fabs(doubleA), 1.0);
//     if (modValue == 0.5) {
//         // tan(pi/2 + n*pi) is undefined, return NaR
//         uA.ui = 0x80;
//         return uA.p;
//     }
    
//     // Calculate tan(pi*x) using standard math library
//     double result = tan(M_PI * doubleA);
    
//     // Convert back to posit
//     return convertDoubleToP8(result);
// } 


#include "p8_LUTs/p8_tan_pi_LUT.h"

posit8_t p8_tan_pi(posit8_t pA) {
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
    uZ.ui = p8tanpi[uA.ui];
    return uZ.p;
}