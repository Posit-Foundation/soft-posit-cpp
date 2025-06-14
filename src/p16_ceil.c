#include <stdbool.h>

#include "internals.h"
#include "platform.h"

posit16_t p16_ceil(posit16_t pA)
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

    // Mask starts at bit 13 for posit16 with es=1
    uint16_t mask  = 0x2000U;
    uint16_t scale = 0U;

    // Determine sign (exclude NaR which is exactly 0x8000)
    bool sign = ui_a > 0x8000U;

    // Work with magnitude only if negative
    if (sign)
    {
        ui_a = (uint16_t) (-(int16_t) ui_a);  // two's complement to get |A|
    }

    uint16_t result;

    if (ui_a == 0)
    {
        // +/-0 stays zero
        return pA;
    }
    else if (ui_a <= 0x4000U)
    {
        // 0 <= |pA| < 1
        result = (sign && (ui_a != 0x4000U)) ? 0x0000U : 0x4000U;
    }
    else if (ui_a <= 0x5000U)
    {
        // 1 <= |pA| < 2
        result = (sign && (ui_a != 0x5000U)) ? 0x4000U : 0x5000U;
    }
    else if (ui_a <= 0x5800U)
    {
        // 2 <= |pA| < 3
        result = (sign && (ui_a != 0x5800U)) ? 0x5000U : 0x5800U;
    }
    else if (ui_a >= 0x7C00U)
    {
        // |pA| >= 256 -> unchanged (also covers NaR earlier)
        return pA;
    }
    else
    {
        // Decode regime/exponent to locate fractional part
        while ((mask & ui_a) != 0)
        {
            scale += 2;
            mask >>= 1;
        }
        mask >>= 1;  // skip regime termination
        if ((mask & ui_a) != 0)
        {
            scale += 1;  // exponent bit
        }
        mask >>= scale;  // align to LSB of integer part

        mask >>= 1;  // first fractional bit
        uint16_t tmp            = ui_a & mask;
        uint16_t bit_n_plus_one = tmp;
        ui_a ^= tmp;              // clear that bit
        tmp = ui_a & (mask - 1);  // remaining fractional bits
        ui_a ^= tmp;              // clear them

        // For positive numbers, if any fractional bits were cleared, round up
        if (!sign && ((bit_n_plus_one | tmp) != 0))
        {
            ui_a = (uint16_t) (ui_a + (mask << 1));
        }
        result = ui_a;
    }

    // Re-apply sign if original number was negative
    if (sign)
    {
        result = (uint16_t) (-(int16_t) result);
    }

    union ui16_p16 uZ;
    uZ.ui = result;
    return uZ.p;
}