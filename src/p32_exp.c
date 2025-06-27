#include "internals.h"
#include "platform.h"

// Helper function for polynomial evaluation used in exponential function
static uint64_t poly(uint64_t f)
{
    uint64_t s;

    // Polynomial coefficients adjusted for 32-bit precision
    s = (f * 0x000001D8B7) >> 32;
    s = (f * (0x0000050159 + s)) >> 26;
    s = (f * (0x00001713E4 + s)) >> 28;
    s = (f * (0x0000400536 + s)) >> 24;
    s = (f * (0x000097D9B0 + s)) >> 22;
    s = (f * (0x00019DBC8 + s)) >> 18;
    s = (f * (0x00055548C + s)) >> 17;
    s = (f * (0x000FFFDA + s)) >> 14;
    s = (f * (0x0017FF80 + s)) >> 11;
    return ((f * (0x0040000 + s)) >> 10) + 0x8000;
}

posit32_t p32_exp(posit32_t pA)
{
    union ui32_p32 uA;
    uint_fast32_t  uiA;
    uint64_t       f;
    int32_t        s;
    uint64_t       bit;
    int_fast32_t   expA;

    uA.p = pA;
    uiA  = uA.ui;
    f    = uiA;

    // Calculate the exponential for given posit pA
    if (uiA < 0x7FFFFF00)
    {
        // Result does not round up to maxpos
        if (uiA < 0x00004000)
        {
            // Small positive values that round to 1
            uA.ui = 0x40000000;  // ONE value in posit32
            return uA.p;
        }

        if (f & 0x40000000)
        {
            // Decode regime for positive regime
            s = 30;
            while (f & 0x20000000)
            {
                f <<= 1;
                s += 2;
            }
        }
        else
        {
            // Decode regime for negative regime
            s = 28;
            while ((f & 0x20000000) == 0)
            {
                f <<= 1;
                s -= 2;
            }
        }

        // Decode exponent (p32 has 2 exponent bits)
        expA = 0;
        if (f & 0x10000000)
            expA += 2;
        if (f & 0x08000000)
            expA += 1;
        s += expA;

        f = (f & 0x07FFFFFF) | 0x08000000;  // Decode fraction
        f = ((s < 0 ? f >> -s : f << s) * 0x0171547A) >> 24;

        uint64_t s_floor = f >> 37;                 // s_floor now stores floor(x)
        f                = poly(f & 0x1FFFFFFFFF);  // Fraction bits of exp(x)
        bit              = (s_floor & 1) << 49;     // First exponent bit of exp(x)
        bit |= ((s_floor & 2) >> 1) << 48;          // Second exponent bit of exp(x)
        s_floor >>= 2;                              // Regime length of exp(x)
        f |= ((0x0100000000000000 << s_floor) - 0x0080000000000000) | bit;

        bit = 1ULL << (36 + s_floor);  // Location of bit n-plus-1
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }

        uA.ui = (f >> (37 + s_floor));  // Return rounded exp(x) as posit
        return uA.p;
    }
    else if (uiA > 0x80000100)
    {
        // Result does not round down to minpos
        if (uiA > 0xFFFFBFFF)
        {
            // Small negative values that round to 1
            uA.ui = 0x40000000;  // ONE value in posit32
            return uA.p;
        }

        if (f & 0x40000000)
        {
            // Decode regime for negative numbers (two's complement)
            s = 29;
            while (f & 0x20000000)
            {
                f <<= 1;
                s -= 2;
            }
        }
        else
        {
            s = 31;
            while ((f & 0x20000000) == 0)
            {
                f <<= 1;
                s += 2;
            }
        }

        // Decode exponent (p32 has 2 exponent bits)
        expA = 0;
        if (f & 0x10000000)
            expA += 2;
        if (f & 0x08000000)
            expA += 1;
        s -= expA;

        f = (f & 0x07FFFFFF) | 0x0FFFFFFF;  // Decode fraction

        if (s < 0)
        {
            f = (f >> -s) | (0x20000000 - (1 << (27 + s)));
        }
        else
        {
            f = (f << s) & 0x1FFFFFFFFF;
        }

        f = (0x0100000000000000 - ((0x20000000 - f) * 0x0171547A)) >> 24;

        int64_t s_floor = (f >> 37) - 64;          // s_floor now stores floor(x)
        f               = poly(f & 0x1FFFFFFFFF);  // Fraction bits of exp(x)
        bit             = (s_floor & 1) << 49;     // First exponent bit of exp(x)
        bit |= ((s_floor & 2) >> 1) << 48;         // Second exponent bit
        uint64_t s_shift = (-1 - s_floor) >> 2;
        f |= 0x0040000000000000 | bit;  // Install regime end bit

        bit = 1ULL << (36 + s_shift);  // Location of bit n-plus-1
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }

        uA.ui = (f >> (37 + s_shift));  // Return rounded exp(x) as posit
        return uA.p;
    }

    // Section for exception cases
    if (uiA < 0x80000000)
    {
        uA.ui = 0x7FFFFFFF;  // Return maxpos
    }
    else if (uiA > 0x80000000)
    {
        uA.ui = 0x00000001;  // Return minpos
    }
    else
    {
        uA.ui = 0x80000000;  // Return NaR
    }

    return uA.p;
}
