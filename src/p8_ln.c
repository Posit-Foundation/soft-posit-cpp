#include <math.h>

#include "internals.h"
#include "platform.h"

// Lookup table for natural logarithm values
static const uint8_t LOG8[128] = {
    128, 144, 148, 152, 154, 156, 157, 158, 159, 161, 165, 168, 170, 173, 175, 178, 180, 182, 183,
    185, 187, 188, 190, 191, 193, 196, 198, 201, 203, 205, 208, 210, 212, 214, 216, 217, 219, 221,
    223, 224, 226, 228, 229, 231, 232, 233, 235, 236, 238, 239, 240, 241, 243, 244, 245, 246, 247,
    249, 250, 251, 252, 253, 254, 255, 0,   2,   4,   6,   8,   9,   11,  13,  14,  16,  17,  19,
    20,  22,  23,  25,  26,  27,  29,  30,  31,  32,  33,  35,  36,  37,  38,  39,  40,  41,  42,
    43,  44,  48,  52,  55,  59,  62,  64,  66,  67,  68,  70,  71,  72,  73,  74,  75,  76,  80,
    84,  87,  89,  92,  94,  96,  97,  98,  100, 101, 102, 105, 108, 112};

posit8_t p8_ln(posit8_t pA)
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
    uZ.ui = LOG8[uA.ui];
    return uZ.p;
}