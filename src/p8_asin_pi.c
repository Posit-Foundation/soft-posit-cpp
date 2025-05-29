#include <math.h>

#include "internals.h"
#include "platform.h"

posit8_t p8_asin_pi(posit8_t pA)
{
    // TODO: Implement proper asin(x)/pi function for posit8_t
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

    // Check if input is in valid range for asin
    if (doubleA < -1.0 || doubleA > 1.0)
    {
        // Outside domain, return NaR
        uA.ui = 0x80;
        return uA.p;
    }

    // Calculate asin(x)/pi using standard math library
    double result = asin(doubleA) / M_PI;

    // Convert back to posit
    return convertDoubleToP8(result);
}