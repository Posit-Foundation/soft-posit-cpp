#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_cos_pi(posit32_t pA)
{
    // TODO: Implement proper cos(pi*x) function for posit32_t
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

    // Calculate cos(pi*x) using standard math library
    double result = cos(M_PI * doubleA);

    // Convert back to posit
    return convertDoubleToP32(result);
}