#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_atan_pi(posit32_t pA)
{
    // TODO: Implement proper atan(x)/pi function for posit32_t
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

    // Calculate atan(x)/pi using standard math library
    // atan is defined for all real values
    double result = atan(doubleA) / M_PI;

    // Convert back to posit
    return convertDoubleToP32(result);
}