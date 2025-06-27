#include <math.h>
#include <stdbool.h>

#include "internals.h"
#include "platform.h"

// Polynomial used by log2 fixed-point evaluation
static uint64_t poly(uint64_t f)
{
    // Ported from Rust implementation
    const uint64_t z   = (f << 29) / (f + 8192ULL);  // fixed-point divide
    const uint64_t zsq = (z * z) >> 30;              // z^2

    uint64_t s = (zsq * 1661ULL) >> 25;           // 1_661
    s          = (zsq * (13209ULL + s)) >> 26;    // 13_209
    s          = (zsq * (75694ULL + s)) >> 24;    // 75_694
    s          = (zsq * (2017019ULL + s)) >> 24;  // 2_017_019
    return (z * (96817627ULL + s)) >> 26;         // 96_817_627
}

posit16_t p16_log2(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint_fast16_t uiA = uA.ui;
    uint64_t      f   = uiA;

    // Return NaR for zero/negative/NaR inputs
    if ((f > 0x7FFFu) || (f == 0))
    {
        uA.ui = 0x8000;
        return uA.p;
    }

    int32_t s;
    if (f & 0x4000u)
    {
        s = 0;
        while (f & 0x2000u)
        {
            f <<= 1;
            s += 2;
        }
    }
    else
    {
        s = -2;
        while ((f & 0x2000u) == 0)
        {
            f <<= 1;
            s -= 2;
        }
    }

    if (f & 0x1000u)
    {
        s += 1;  // exponent bit
    }

    f &= 0x0FFFu;  // fraction without hidden bit
    if (f != 0)
    {
        f = poly(f);
    }

    f |= ((uint64_t) ((s < 0) ? (64 + s) : s)) << 28;

    bool sign = (f & 0x000200000000ULL) != 0;
    if (sign)
    {
        f = 0x000400000000ULL - f;  // absolute value
    }

    if (f < 0x10000000ULL)
    {
        if (f != 0)
        {
            s = 30;
            while ((f & 0x08000000ULL) == 0)
            {
                f <<= 1;
                s += 1;
            }
            f = (f ^ 0x18000000ULL) | (((uint64_t) (1 ^ (s & 1)) << 27));
            s >>= 1;
            uint64_t bit = 1ULL << (s - 1);
            if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
            {
                f += bit;
            }
            f >>= s;
        }
    }
    else
    {
        s = 0;
        while (f > 0x1FFFFFFFULL)
        {
            f = (f & 1ULL) | (f >> 1);
            s += 1;
        }
        f &= 0x0FFFFFFFULL;
        if (s & 1)
        {
            f |= 0x10000000ULL;
        }
        s >>= 1;
        f |= ((0x80000000ULL << s) - 0x40000000ULL);
        uint64_t bit = 0x8000ULL << s;
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }
        f >>= s + 16;
    }

    if (sign)
    {
        f = 0x00010000ULL - f;  // restore sign
    }

    uA.ui = (uint_fast16_t) f;
    return uA.p;
}