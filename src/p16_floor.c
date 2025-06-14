#include <stdbool.h>

#include "internals.h"
#include "platform.h"

posit16_t p16_floor(posit16_t pA)
{
    // Convert to raw bits
    union ui16_p16 uA;
    uA.p          = pA;
    uint16_t ui_a = uA.ui;

    // NaR remains unchanged
    if (ui_a == 0x8000)
    {
        return pA;
    }

    // Mask starts at bit 13 (for 16-bit posit, es=1)
    uint16_t mask  = 0x2000U;
    uint16_t scale = 0U;

    // Determine sign (exclude NaR which is exactly 0x8000)
    bool sign = ui_a > 0x8000U;

    // Work with magnitude only if the number is negative
    if (sign)
    {
        ui_a = (uint16_t) (-(int16_t) ui_a);  // two's complement to get |A|
    }

    uint16_t result;

    if (ui_a < 0x4000U)
    {
        // 0 <= |pA| < 1
        result = (sign && (ui_a != 0)) ? 0x4000U : 0x0000U;
    }
    else if (ui_a < 0x5000U)
    {
        // 1 <= |pA| < 2
        result = (sign && (ui_a != 0x4000U)) ? 0x5000U : 0x4000U;
    }
    else if (ui_a < 0x5800U)
    {
        // 2 <= |pA| < 3
        result = (sign && (ui_a != 0x5000U)) ? 0x5800U : 0x5000U;
    }
    else if (ui_a >= 0x7C00U)
    {
        // |pA| >= 256 -> unchanged (also covers NaR which was handled earlier)
        return pA;
    }
    else
    {
        // Decode regime/exponent to identify integer bit position
        while ((mask & ui_a) != 0)
        {
            scale += 2;  // each regime bit adds 2 to scale (es=1)
            mask >>= 1;
        }
        mask >>= 1;  // skip regime termination bit
        if ((mask & ui_a) != 0)
        {
            scale += 1;  // exponent bit contributes +1
        }
        mask >>= scale;  // align to LSB of integer part

        mask >>= 1;  // move to first fractional bit
        uint16_t tmp            = ui_a & mask;
        uint16_t bit_n_plus_one = tmp;  // non-zero if fractional part had 1 at this bit
        ui_a ^= tmp;                    // clear that bit
        tmp = ui_a & (mask - 1);        // all remaining fractional bits
        ui_a ^= tmp;                    // clear them too

        // For negative numbers, if any fractional bits were cleared, round away
        // from zero
        if (sign && ((bit_n_plus_one | tmp) != 0))
        {
            ui_a = (uint16_t) (ui_a + (mask << 1));
        }
        result = ui_a;
    }

    // Restore sign if the original number was negative
    if (sign)
    {
        result = (uint16_t) (-(int16_t) result);
    }

    union ui16_p16 uZ;
    uZ.ui = result;
    return uZ.p;
}