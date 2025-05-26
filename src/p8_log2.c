#include "internals.h"
#include "platform.h"
#include <math.h>

// posit8_t p8_log2(posit8_t pA) {
//     union ui8_p8 uA;
//     double doubleA;
    
//     uA.p = pA;
    
//     // Check for special cases
//     if (uA.ui == 0x80) {
//         // If NaR, return NaR
//         return pA;
//     }
    
//     // Handle non-positive values
//     if (uA.ui <= 0x40) {
//         if (uA.ui == 0x40) {
//             // log2(1) = 0
//             uA.ui = 0;
//             return uA.p;
//         } else {
//             // Negative or zero input returns NaR for logarithm
//             uA.ui = 0x80;
//             return uA.p;
//         }
//     }
    
//     // Convert posit to double
//     doubleA = convertP8ToDouble(pA);
    
//     // Calculate base-2 logarithm using standard math library
//     double result = log2(doubleA);
    
//     // Convert back to posit
//     return convertDoubleToP8(result);
// } 

#include "p8_LUTs/p8_log2_LUT.h"

posit8_t p8_log2(posit8_t pA) {
  union ui8_p8 uA;
  uA.p = pA;

  // Check if input is negative or NaR (value > 127)
  if (uA.ui > 127) {
    // Return NaR for negative values or NaR
    uA.ui = 0x80; // NaR value
    return uA.p;
  }

  // Look up the result in the table
  union ui8_p8 uZ;
  uZ.ui = p8log2[uA.ui];
  return uZ.p;
}