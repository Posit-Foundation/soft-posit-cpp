#include <math.h>

#include "internals.h"
#include "platform.h"

// Polynomial helper for exp2
static uint64_t poly(uint64_t f)
{
    uint64_t s = (f * (0x9BA00000ULL + (f * 491ULL))) >> 34;
    s          = (f * (0x0013F840ULL + s)) >> 20;
    s          = (f * (0x00718A80ULL + s)) >> 16;
    s          = (f * (0x1EC04000ULL + s)) >> 21;
    return (f * (0x2C5C8000ULL + s)) >> 24;
}

posit16_t p16_exp2(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint_fast16_t uiA = uA.ui;
    uint64_t      f   = uiA;
    int32_t       s;
    uint64_t      bit;

    // Main approximation paths (mirrors Rust logic)
    if (uiA < 29377)
    {
        // Does not round up to maxpos
        if (uiA < 221)
        {
            uA.ui = 0x4000;  // ONE
            return uA.p;
        }

        if (f & 0x4000u)
        {
            s = 8;
            while (f & 0x2000u)
            {
                f <<= 1;
                s += 2;
            }
        }
        else
        {
            s = 6;
            while ((f & 0x2000u) == 0)
            {
                f <<= 1;
                s -= 2;
            }
        }

        if (f & 0x1000u)
        {
            s += 1;
        }
        f = (f & 0x0FFFu) | 0x1000u;  // fraction with hidden bit
        f = (s < 0) ? (f >> -s) : (f << s);

        uint64_t s_floor = f >> 20;                  // floor(x)
        f                = poly(f & 0x000FFFFFULL);  // fraction bits of exp2
        bit              = (s_floor & 1ULL) << 26;   // exponent bit
        s_floor >>= 1;                               // regime length
        f |= ((0x20000000ULL << s_floor) - 0x10000000ULL) | bit;

        bit = 1ULL << (13 + s_floor);  // n+1
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }

        uA.ui = (uint_fast16_t) (f >> (14 + s_floor));
        return uA.p;
    }
    else if (uiA > 36159)
    {
        if (uiA > 65379)
        {
            uA.ui = 0x4000;  // ONE (round up to 1)
            return uA.p;
        }

        if (f & 0x4000u)
        {
            s = 7;
            while (f & 0x2000u)
            {
                f <<= 1;
                s -= 2;
            }
        }
        else
        {
            s = 9;
            while ((f & 0x2000u) == 0)
            {
                f <<= 1;
                s += 2;
            }
        }

        if (f & 0x1000u)
        {
            s -= 1;
        }
        f = (f & 0x0FFFu) | 0x01FFE000u;
        if (s < 0)
        {
            f = (f >> -s) | (0x02000000ULL - (1ULL << (13 + s)));
        }
        else
        {
            f = (f << s) & 0x01FFFFFFULL;
        }

        uint64_t s_floor = (f >> 20) - 32ULL;
        f                = poly(f & 0x000FFFFFULL);
        bit              = (s_floor & 1ULL) << 26;
        uint64_t s_shift = (uint64_t) ((-1LL - (int64_t) s_floor) >> 1);
        f |= 0x08000000ULL | bit;

        bit = 1ULL << (13 + s_shift);
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }

        uA.ui = (uint_fast16_t) (f >> (14 + s_shift));
        return uA.p;
    }

    // Exception cases
    if (uiA < 0x8000)
    {
        uA.ui = 0x7FFF;  // maxpos
    }
    else if (uiA > 0x8000)
    {
        uA.ui = 0x0001;  // minpos
    }
    else
    {
        uA.ui = 0x8000;  // NaR
    }
    return uA.p;
}