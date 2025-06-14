#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_acos_pi(posit32_t pA)
{
    // TODO: Implement proper acos(x)/pi function for posit32_t
    // Currently using double conversion as placeholder
    union ui32_p32 uA;
    double         doubleA;

    uA.p = pA;

    // Check for special cases
    if (uA.ui == 0x80000000)
    {
        // If NaR, return NaR
        return pA;
    }

    // Convert posit to double
    doubleA = convertP32ToDouble(pA);

    // Check if input is in valid range for acos
    if (doubleA < -1.0 || doubleA > 1.0)
    {
        // Outside domain, return NaR
        uA.ui = 0x80000000;
        return uA.p;
    }

    // Calculate acos(x)/pi using standard math library
    double result = acos(doubleA) / M_PI;

    // Convert back to posit
    return convertDoubleToP32(result);
}