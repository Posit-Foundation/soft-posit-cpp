#include <math.h>
#include <stdint.h>

#include "internals.h"
#include "platform.h"

// --------------------------- helper functions ----------------------------
static inline uint64_t isqrt_u64(uint64_t x)
{
    uint64_t res = 0;
    uint64_t bit = 1ULL << 62;
    while (bit > x)
        bit >>= 2;
    while (bit != 0)
    {
        if (x >= res + bit)
        {
            x -= res + bit;
            res = (res >> 1) + bit;
        }
        else
        {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

static inline uint64_t poly_atan(uint64_t f)
{
    uint64_t fsq = (f * f) >> 28;
    uint64_t s   = (fsq * 6969ull) >> 24;
    s            = (fsq * (530432ull - s)) >> 28;
    s            = (fsq * (1273944ull - s)) >> 28;
    s            = (fsq * (2358656ull - s)) >> 27;
    s            = (fsq * (9340208ull - s)) >> 29;
    s            = (fsq * (17568064ull - s)) >> 24;
    return ((f + 1) << 30) / (843315168ull + s);
}

posit16_t p16_atan_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint64_t f = (uint64_t) uA.ui;

    // Handle NaR and zero quickly
    if ((f & 0x7FFFu) == 0)
    {
        return pA;  // returns NaR or 0 unchanged
    }

    bool neg = ((uA.ui >> 15) != 0);
    if (neg)
    {
        f = 0x10000 - f;  // absolute value
    }

    int32_t s;
    if (f < 0x4000u)
    {
        // SE quadrant (|x| < 1)
        if (f > 4925u)
        {
            s = 14;
            while ((f & 0x2000u) == 0)
            {
                f <<= 1;
                s -= 2;
            }
            if (f & 0x1000u)
                s += 1;
            f = (f & 0x0FFFu) | 0x1000u;
            f <<= s;
            f = poly_atan(f);
        }
        else
        {
            s = 13;
            while ((f & 0x1000u) == 0)
            {
                f <<= 1;
                s -= 2;
            }
            if (f & 0x800u)
                s += 1;
            f = (f & 0x7FFu) | 0x800u;
            if (s < 0)
                f >>= -s;
            else
                f <<= s;

            // small x approximation: f = (f << 30) / ((((f*f)>>34)*67)+843314118)
            uint64_t numerator = f << 30;
            uint64_t denom     = (((f * f) >> 34) * 67ull) + 843314118ull;
            f                  = numerator / denom;
        }
    }
    else
    {
        // NE quadrant (|x| >=1)
        if (f < 27109u)
        {
            s = 0;
            while ((f & 0x2000u) != 0)
            {
                f <<= 1;
                s += 2;
            }
            if (f & 0x1000u)
                s += 1;
            f = (f & 0x0FFFu) | 0x1000u;
            f <<= s;
            // fixed reciprocal
            f = 0x010000000000ULL / f;  // 0x0100_0000_0000
            f = 0x20000000ull - poly_atan(f);
        }
        else
        {
            s = -1;
            while ((f & 0x1000u) != 0)
            {
                f <<= 1;
                s += 2;
            }
            if (f & 0x800u)
                s += 1;
            f = (f & 0x7FFu) | 0x800u;
            f <<= s;
            // large value approximation
            uint64_t numerator = 0x00028BE5FF800000ULL;
            uint64_t denom     = (f << 13) + (0x0AA55000ULL / f);
            f                  = 0x20000000ull - (numerator / denom);
        }
    }

    // convert fixed-point to posit
    if (f > 1)
    {
        s = 34;
        while ((f & 0x20000000ull) == 0)
        {
            f <<= 1;
            s += 1;
        }
        f = (f ^ 0x60000000ull) | ((uint64_t) (1 ^ (s & 1)) << 29);
        s >>= 1;
        uint64_t bit = 1ull << (s - 1);
        if (f & bit)
        {
            if (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0))
            {
                f += bit;
            }
        }
        f >>= s;
    }

    uA.ui = (uint16_t) (neg ? (0x10000u - f) : f);
    return uA.p;
}