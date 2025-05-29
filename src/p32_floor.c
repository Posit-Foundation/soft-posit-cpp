#include <stdbool.h>

#include "internals.h"
#include "platform.h"

posit32_t p32_floor(posit32_t pA)
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

    // Mask starts at bit 29 for 32-bit posit (es = 2)
    uint32_t mask  = 0x20000000U;
    uint32_t scale = 0U;

    // Determine sign (exclude NaR which is exactly 0x80000000)
    bool sign = ui_a > 0x80000000U;

    // Work with magnitude only if the number is negative
    if (sign)
    {
        ui_a = (uint32_t) (-(int32_t) ui_a);
    }

    uint32_t result;

    if (ui_a < 0x40000000U)
    {
        // 0 <= |pA| < 1
        result = (sign && (ui_a != 0)) ? 0x40000000U : 0x00000000U;
    }
    else if (ui_a < 0x48000000U)
    {
        // 1 <= |pA| < 2
        result = (sign && (ui_a != 0x40000000U)) ? 0x48000000U : 0x40000000U;
    }
    else if (ui_a < 0x4C000000U)
    {
        // 2 <= |pA| < 3
        result = (sign && (ui_a != 0x48000000U)) ? 0x4C000000U : 0x48000000U;
    }
    else if (ui_a >= 0x7E800000U)
    {
        // |pA| is already an integer (or very large) – leave unchanged
        return pA;
    }
    else
    {
        // Decode regime/exponent to identify integer bit position
        while ((mask & ui_a) != 0)
        {
            scale += 4;  // each regime bit adds 4 to scale (es = 2)
            mask >>= 1;
        }

        mask >>= 1;  // skip regime termination bit

        // Process the two exponent bits (es = 2)
        if ((mask & ui_a) != 0)
            scale += 2;
        mask >>= 1;
        if ((mask & ui_a) != 0)
            scale += 1;

        // Align mask to the least-significant bit of the integer part
        mask >>= scale;

        // Move to the first fractional bit (bit n+1)
        mask >>= 1;

        uint32_t tmp            = ui_a & mask;
        uint32_t bit_n_plus_one = tmp;  // non-zero if this fractional bit is 1
        ui_a ^= tmp;                    // clear that bit

        tmp = ui_a & (mask - 1);  // all remaining fractional bits
        ui_a ^= tmp;              // clear them too

        // For negative numbers, if any fractional bits were cleared, round away
        // from zero (i.e., make the magnitude larger by 1)
        if (sign && ((bit_n_plus_one | tmp) != 0))
        {
            ui_a = ui_a + (mask << 1);
        }

        result = ui_a;
    }

    // Restore sign if the original number was negative
    if (sign)
    {
        result = (uint32_t) (-(int32_t) result);
    }

    union ui32_p32 uZ;
    uZ.ui = result;
    return uZ.p;
}