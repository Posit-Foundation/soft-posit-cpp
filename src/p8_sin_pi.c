#include <math.h>

#include "internals.h"
#include "platform.h"

// posit8_t p8_sin_pi(posit8_t pA) {

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

//     // Calculate sin(pi*x) using standard math library
//     double result = sin(M_PI * doubleA);

//     // Convert back to posit
//     return convertDoubleToP8(result);
// }

static const uint8_t p8sinpi[256] = {
    0,   3,   6,   9,   12,  16,  19,  22,  24,  27,  30,  33,  36,  38,  41,  43,  45,  47,  49,
    51,  53,  55,  56,  58,  59,  60,  61,  62,  63,  63,  64,  64,  64,  64,  64,  63,  63,  62,
    61,  60,  59,  58,  56,  55,  53,  51,  49,  47,  45,  43,  41,  38,  36,  33,  30,  27,  24,
    22,  19,  16,  12,  9,   6,   3,   0,   250, 244, 237, 232, 226, 220, 215, 211, 207, 203, 200,
    197, 195, 193, 192, 192, 192, 193, 195, 197, 200, 203, 207, 211, 215, 220, 226, 232, 237, 244,
    250, 0,   24,  45,  59,  64,  59,  45,  24,  0,   232, 211, 197, 192, 197, 211, 232, 0,   64,
    0,   192, 0,   64,  0,   192, 0,   0,   0,   0,   0,   0,   0,   0,   128, 0,   0,   0,   0,
    0,   0,   0,   0,   64,  0,   192, 0,   64,  0,   192, 0,   24,  45,  59,  64,  59,  45,  24,
    0,   232, 211, 197, 192, 197, 211, 232, 0,   6,   12,  19,  24,  30,  36,  41,  45,  49,  53,
    56,  59,  61,  63,  64,  64,  64,  63,  61,  59,  56,  53,  49,  45,  41,  36,  30,  24,  19,
    12,  6,   0,   253, 250, 247, 244, 240, 237, 234, 232, 229, 226, 223, 220, 218, 215, 213, 211,
    209, 207, 205, 203, 201, 200, 198, 197, 196, 195, 194, 193, 193, 192, 192, 192, 192, 192, 193,
    193, 194, 195, 196, 197, 198, 200, 201, 203, 205, 207, 209, 211, 213, 215, 218, 220, 223, 226,
    229, 232, 234, 237, 240, 244, 247, 250, 253,
};

posit8_t p8_sin_pi(posit8_t pA)
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
    uZ.ui = p8sinpi[uA.ui];
    return uZ.p;
}