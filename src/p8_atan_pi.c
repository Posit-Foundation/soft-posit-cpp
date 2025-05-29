#include <math.h>

#include "internals.h"
#include "platform.h"

posit8_t p8_atan_pi(posit8_t pA)
{
    // TODO: Implement proper atan(x)/pi function for posit8_t
    // Currently using double conversion as placeholder
    union ui8_p8 uA;
    double       doubleA;

    uA.p = pA;

    // Check for special cases
    if (uA.ui == 0x80)
    {
        // If NaR, return NaR
        return pA;
    }

    // Convert posit to double
    doubleA = convertP8ToDouble(pA);

    // Calculate atan(x)/pi using standard math library
    // atan is defined for all real values
    double result = atan(doubleA) / M_PI;

    // Convert back to posit
    return convertDoubleToP8(result);
}