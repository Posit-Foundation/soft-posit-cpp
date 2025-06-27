#include <math.h>

#include "internals.h"
#include "platform.h"

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

static const uint8_t p8log2[128] = {
    128, 140, 142, 143, 144, 147, 149, 150, 152, 153, 155, 156, 157, 158, 158, 159, 160, 163, 165,
    168, 170, 173, 175, 177, 179, 181, 182, 184, 186, 187, 189, 191, 192, 195, 198, 200, 203, 205,
    208, 210, 213, 215, 217, 219, 221, 223, 226, 227, 229, 231, 233, 235, 237, 239, 240, 242, 244,
    245, 247, 248, 250, 252, 253, 255, 0,   3,   6,   8,   11,  13,  16,  18,  21,  23,  25,  27,
    29,  31,  34,  35,  37,  39,  41,  43,  45,  47,  48,  50,  52,  53,  55,  56,  58,  60,  61,
    63,  64,  67,  69,  72,  74,  77,  79,  81,  83,  85,  86,  88,  90,  91,  93,  95,  96,  97,
    99,  100, 101, 102, 102, 103, 104, 107, 109, 110, 112, 113, 114, 116,
};

posit8_t p8_log2(posit8_t pA)
{
    union ui8_p8 uA;
    uA.p = pA;

    // Check if input is negative or NaR (value > 127)
    if (uA.ui > 127)
    {
        // Return NaR for negative values or NaR
        uA.ui = 0x80;  // NaR value
        return uA.p;
    }

    // Look up the result in the table
    union ui8_p8 uZ;
    uZ.ui = p8log2[uA.ui];
    return uZ.p;
}