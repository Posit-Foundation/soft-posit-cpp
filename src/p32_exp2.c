#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_exp2(posit32_t pA)
{
    // TODO: Implement proper exp2 function for posit32_t
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

    // Calculate 2^x using standard math library
    double result = pow(2.0, doubleA);

    // Convert back to posit
    return convertDoubleToP32(result);
}