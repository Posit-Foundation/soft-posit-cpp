#include <math.h>

#include "internals.h"
#include "platform.h"

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

static const uint8_t p8exp2[256] = {
    64,  64,  65,  65,  65,  66,  66,  67,  67,  67,  68,  68,  68,  69,  69,  70,  70,  70,  71,
    71,  72,  72,  73,  73,  73,  74,  74,  75,  75,  76,  76,  77,  77,  78,  78,  79,  79,  80,
    80,  81,  81,  82,  82,  83,  84,  84,  85,  85,  86,  86,  87,  88,  88,  89,  89,  90,  91,
    91,  92,  93,  93,  94,  95,  95,  96,  96,  97,  97,  97,  98,  98,  99,  99,  99,  100, 100,
    101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 109, 109, 110, 111,
    111, 112, 113, 114, 114, 115, 116, 117, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 125,
    126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 128, 0,   0,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   3,   4,   4,   5,   5,   6,   6,   7,   7,
    8,   9,   10,  10,  11,  12,  13,  15,  16,  16,  17,  17,  17,  18,  18,  19,  19,  19,  20,
    20,  21,  21,  22,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  29,  29,  30,
    31,  31,  32,  32,  33,  33,  33,  34,  34,  35,  35,  35,  36,  36,  36,  37,  37,  38,  38,
    38,  39,  39,  40,  40,  41,  41,  41,  42,  42,  43,  43,  44,  44,  45,  45,  46,  46,  47,
    47,  48,  48,  49,  49,  50,  50,  51,  52,  52,  53,  53,  54,  54,  55,  56,  56,  57,  57,
    58,  59,  59,  60,  61,  61,  62,  63,  63,
};

posit8_t p8_exp2(posit8_t pA)
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

    // Special case: if input is +0, return exact 1.0 (exp2(0) = 1)
    if (uA.ui == 0x00)
    {
        union ui8_p8 uOne;
        uOne.ui = 0x40;  // Posit8 bit pattern for 1.0
        return uOne.p;
    }

    // Look up the result from the table
    union ui8_p8 uZ;
    uZ.ui = p8exp2[uA.ui];
    return uZ.p;
}