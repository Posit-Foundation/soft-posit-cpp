#include <stdbool.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_ceil(posit32_t pA)
{
    // Convert to raw bits
    union ui32_p32 uA;
    uA.p          = pA;
    uint32_t ui_a = uA.ui;

    // NaR remains unchanged
    if (ui_a == 0x80000000U)
    {
        return pA;
    }

    // Starting mask: bit 29 for posit32, es = 2
    uint32_t mask  = 0x20000000U;
    uint32_t scale = 0U;

    // Determine sign (exclude NaR which is exactly 0x80000000)
    bool sign = ui_a > 0x80000000U;

    // Work with magnitude only if negative
    if (sign)
    {
        ui_a = (uint32_t) (-(int32_t) ui_a);
    }

    uint32_t result;

    if (ui_a == 0)
    {
        // ±0 stays zero
        return pA;
    }
    else if (ui_a <= 0x40000000U)
    {
        // 0 <= |pA| < 1
        result = (sign && (ui_a != 0x40000000U)) ? 0x00000000U : 0x40000000U;
    }
    else if (ui_a <= 0x48000000U)
    {
        // 1 <= |pA| < 2
        result = (sign && (ui_a != 0x48000000U)) ? 0x40000000U : 0x48000000U;
    }
    else if (ui_a <= 0x4C000000U)
    {
        // 2 <= |pA| < 3
        result = (sign && (ui_a != 0x4C000000U)) ? 0x48000000U : 0x4C000000U;
    }
    else if (ui_a >= 0x7E800000U)
    {
        // |pA| is already an integer (or very large) – leave unchanged
        return pA;
    }
    else
    {
        // Decode regime/exponent to locate fractional part
        while ((mask & ui_a) != 0)
        {
            scale += 4;  // each regime bit contributes 4 to scale (es = 2)
            mask >>= 1;
        }

        mask >>= 1;  // skip regime termination bit

        // Two-bit exponent (es = 2)
        if ((mask & ui_a) != 0)
            scale += 2;
        mask >>= 1;
        if ((mask & ui_a) != 0)
            scale += 1;

        // Align mask to the least-significant bit of the integer part
        mask >>= scale;

        // Move mask to the first fractional bit (bit n+1)
        mask >>= 1;

        uint32_t tmp            = ui_a & mask;
        uint32_t bit_n_plus_one = tmp;  // non-zero if this fractional bit is 1
        ui_a ^= tmp;                    // clear that bit

        tmp = ui_a & (mask - 1);  // remaining fractional bits
        ui_a ^= tmp;              // clear them as well

        // For positive numbers, if any fractional bits were cleared, round up
        if (!sign && ((bit_n_plus_one | tmp) != 0))
        {
            ui_a = ui_a + (mask << 1);
        }

        result = ui_a;
    }

    // Re-apply sign if the original number was negative
    if (sign)
    {
        result = (uint32_t) (-(int32_t) result);
    }

    union ui32_p32 uZ;
    uZ.ui = result;
    return uZ.p;
}