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
    {
        bit >>= 2;
    }
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

static inline uint64_t poly_asin(uint64_t f)
{
    uint64_t fsq = (f * f) >> 28;
    uint64_t s   = 13944ull + ((fsq * 3855ull) >> 26);
    s            = 100344ull + ((fsq * s) >> 26);
    s            = 1780112ull + ((fsq * s) >> 25);
    s            = 42722832ull + ((fsq * s) >> 26);
    return (f * s) >> 25;
}

posit16_t p16_asin_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint64_t f = (uint64_t) uA.ui;

    // Handle 0 and NaR cases quickly
    if ((uA.ui & 0x7FFF) == 0)
    {
        return pA;  // returns self (covers 0 and NaR)
    }

    bool neg = ((uA.ui >> 15) != 0);
    if (neg)
    {
        f = 0x10000 - f;  // absolute value
    }

    if (f > 0x4000u)
    {
        uA.ui = 0x8000;  // NaR for |x|>1
        return uA.p;
    }

    int32_t s;
    if (f < 0x3000u)
    {
        // |x| < 0.5
        s = 14;
        while ((f & 0x2000u) == 0)
        {
            f <<= 1;
            s -= 2;
        }
        if (f & 0x1000u)
        {
            s += 1;
        }
        f = (f & 0xFFFu) | 0x1000u;
        if (s < 0)
            f >>= -s;
        else
            f <<= s;
        f = poly_asin(f);
    }
    else
    {
        uint64_t tmp  = (0x4000u - f) << 42;
        uint64_t root = isqrt_u64(tmp);
        f             = 0x20000000ull - (poly_asin(root) << 1);
    }

    // Convert fixed-point to posit form
    s = 34;
    if (f > 4)
    {
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

    uint16_t res_ui = (uint16_t) (neg ? (0x10000u - f) : f);
    uA.ui           = res_ui;
    return uA.p;
}