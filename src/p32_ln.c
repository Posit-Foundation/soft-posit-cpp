#include <math.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_ln(posit32_t pA)
{
    // TODO: Implement proper natural logarithm function for posit32_t
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

    // Handle non-positive values
    if (uA.ui <= 0x40000000)
    {
        if (uA.ui == 0x40000000)
        {
            // ln(1) = 0
            uA.ui = 0;
            return uA.p;
        }
        else
        {
            // Negative or zero input returns NaR for logarithm
            uA.ui = 0x80000000;
            return uA.p;
        }
    }

    // Convert posit to double
    doubleA = convertP32ToDouble(pA);

    // Calculate natural logarithm using standard math library
    double result = log(doubleA);

    // Convert back to posit
    return convertDoubleToP32(result);
}