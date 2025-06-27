#include <math.h>

#include "internals.h"
#include "platform.h"

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

static const uint8_t p8tanpi[256] = {
    0,   3,   6,   9,   13,  16,  19,  23,  27,  30,  34,  38,  43,  47,  53,  58,  64,  67,  71,
    75,  80,  85,  92,  97,  99,  102, 106, 112, 114, 117, 121, 125, 128, 131, 135, 139, 142, 144,
    150, 154, 157, 159, 164, 171, 176, 181, 185, 189, 192, 198, 203, 209, 213, 218, 222, 226, 229,
    233, 237, 240, 243, 247, 250, 253, 0,   6,   13,  19,  27,  34,  43,  53,  64,  71,  80,  92,
    99,  106, 114, 121, 128, 135, 142, 150, 157, 164, 176, 185, 192, 203, 213, 222, 229, 237, 243,
    250, 0,   27,  64,  99,  128, 157, 192, 229, 0,   27,  64,  99,  128, 157, 192, 229, 0,   128,
    0,   128, 0,   128, 0,   128, 0,   0,   0,   0,   0,   0,   0,   0,   128, 0,   0,   0,   0,
    0,   0,   0,   0,   128, 0,   128, 0,   128, 0,   128, 0,   27,  64,  99,  128, 157, 192, 229,
    0,   27,  64,  99,  128, 157, 192, 229, 0,   6,   13,  19,  27,  34,  43,  53,  64,  71,  80,
    92,  99,  106, 114, 121, 128, 135, 142, 150, 157, 164, 176, 185, 192, 203, 213, 222, 229, 237,
    243, 250, 0,   3,   6,   9,   13,  16,  19,  23,  27,  30,  34,  38,  43,  47,  53,  58,  64,
    67,  71,  75,  80,  85,  92,  97,  99,  102, 106, 112, 114, 117, 121, 125, 128, 131, 135, 139,
    142, 144, 150, 154, 157, 159, 164, 171, 176, 181, 185, 189, 192, 198, 203, 209, 213, 218, 222,
    226, 229, 233, 237, 240, 243, 247, 250, 253,
};

posit8_t p8_tan_pi(posit8_t pA)
{
    union ui8_p8 uA;
    uA.p = pA;

    // Check if input is NaR
    if (uA.ui == 128)
    {
        // Return NaR for NaR
        uA.ui = 0x80;  // NaR value
        return uA.p;
    }

    // Look up the result from the table
    union ui8_p8 uZ;
    uZ.ui = p8tanpi[uA.ui];
    return uZ.p;
}