#include <math.h>
#include <stdint.h>

#include "internals.h"
#include "platform.h"

// Compute acos(x)/pi for a posit16_t value.

// --------------------------- helper functions ----------------------------

static inline uint64_t isqrt_u64(uint64_t x)
{
    // Integer square-root via binary search (works up to 64-bit inputs)
    uint64_t res = 0;
    uint64_t bit = 1ULL << 62;  // The second-to-top bit
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

static inline uint64_t poly(uint64_t f)
{
    uint64_t fsq   = (f * f) >> 28;
    uint64_t s_val = 13696ull + ((fsq * 7955ull) >> 27);
    s_val          = 100510ull + ((fsq * s_val) >> 26);
    s_val          = 1780047ull + ((fsq * s_val) >> 25);
    s_val          = 42722829ull + ((fsq * s_val) >> 26);
    return 0x20000000ull - ((f * s_val) >> 25);
}

static inline uint64_t to_fixed28_acos_pi(uint64_t i)
{
    int32_t  s = 14;
    uint64_t f = i;
    while ((f & 0x2000ull) == 0)
    {
        f <<= 1;
        s -= 2;
    }
    if (f & 0x1000ull)
    {
        s += 1;
    }
    f = (f & 0xFFFull) | 0x1000ull;
    return f << s;
}

posit16_t p16_acos_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint64_t f = (uint64_t) uA.ui;  // work in 64-bit for head-room

    // If |x| > 1 we return NaR (Not a Real)
    if ((f > 0x4000) && (f < 0xC000))
    {
        uA.ui = 0x8000;  // NaR representation for posit16
        return uA.p;
    }

    // For very small |x| values the result rounds to 0.5 exactly
    if (f < 165u || f > 65307u)
    {
        uA.ui = 0x3000;  // 0.5 in posit-16 (es = 1)
        return uA.p;
    }

    // Helper lambdas implemented as static inline functions above

    if (f < 0x3000)
    {
        // input < 0.5
        f = poly(to_fixed28_acos_pi(f));
    }
    else if (f < 0x4001)
    {
        // near 0.5 .. 1
        uint64_t tmp  = (0x4000 - f) << 42;
        uint64_t root = isqrt_u64(tmp);
        f             = 0x40000000ULL - (poly(root) << 1);
    }
    else if (f > 53248u)
    {
        // input near -0.5 .. -1
        f = 0x40000000ULL - poly(to_fixed28_acos_pi(0x10000 - f));
    }
    else
    {
        // remaining range
        uint64_t tmp  = (f - 0xC000) << 42;
        uint64_t root = isqrt_u64(tmp);
        f             = poly(root) << 1;
    }

    int s = 35;  // number of fractional bits to discard when converting to posit
    if (f > 1)
    {
        while ((f & 0x40000000ULL) == 0)
        {
            f <<= 1;
            s += 1;
        }
        // encode sign/exponent information into the remaining 32 bits
        f = (f ^ 0xC0000000ULL) | ((uint64_t) (1 ^ (s & 1)) << 30);
        s >>= 1;
        uint64_t bit = 1ull << (s - 1);
        if ((f & bit) != 0)
        {
            // round to nearest, tie to even
            if (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0))
            {
                f += bit;
            }
        }
        f >>= s;
    }

    uA.ui = (uint16_t) f;
    return uA.p;
}