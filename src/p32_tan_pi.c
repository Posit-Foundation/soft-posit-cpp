#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_tan_pi(posit32_t pA)
{
    // TODO: Implement proper tan(pi*x) function for posit32_t
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

    // Handle special cases for tan
    double modValue = fmod(fabs(doubleA), 1.0);
    if (modValue == 0.5)
    {
        // tan(pi/2 + n*pi) is undefined, return NaR
        uA.ui = 0x80000000;
        return uA.p;
    }

    // Calculate tan(pi*x) using standard math library
    double result = tan(M_PI * doubleA);

    // Convert back to posit
    return convertDoubleToP32(result);
}