#include <math.h>
#include <stdbool.h>

#include "internals.h"
#include "platform.h"

// Helper polynomial used for ln in fixed-point path
static uint64_t poly(uint64_t f)
{
    // Ported from the Rust implementation
    const uint64_t z = ((f << 31) + 2) / (f + 8192);  // fixed-point divide – remainder discarded
    const uint64_t zsq = (z * z) >> 30;               // z^2 in fixed-point

    uint64_t s = (zsq * 1584ULL) >> 28;
    s          = (zsq * (26661ULL + s)) >> 29;
    s          = (zsq * (302676ULL + s)) >> 27;
    s          = (zsq * (16136153ULL + s)) >> 30;
    return (z * (193635259ULL + s)) >> 27;  // returned 31-bit fixed-point value
}

posit16_t p16_ln(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint_fast16_t uiA = uA.ui;
    uint64_t      f   = uiA;  // promote to 64-bit for intermediate head-room

    // Special-case handling – NaR / zero / negative inputs
    if ((f > 0x7FFFu) || (f == 0))
    {
        uA.ui = 0x8000;  // NaR
        return uA.p;
    }

    int32_t s;
    if (f & 0x4000u)
    {
        // Decode regime for numbers ≥1
        s = 0;
        while (f & 0x2000u)
        {
            f <<= 1;
            s += 2;
        }
    }
    else
    {
        // Decode regime for numbers <1
        s = -2;
        while ((f & 0x2000u) == 0)
        {
            f <<= 1;
            s -= 2;
        }
    }

    if (f & 0x1000u)
    {
        s += 1;  // decode exponent bit
    }

    // Obtain 12-bit fraction without hidden bit
    f &= 0x0FFFu;
    if (f != 0)
    {
        f = poly(f);  // turn fraction into mantissa of logarithm
    }

    // Append integer component (base-2 exponent) to top of fixed-point value
    f |= ((uint64_t) ((s < 0) ? (64 + s) : s)) << 30;

    // Convert from log2 to ln by multiplying with ln(2) ≈ 0.693147...
    if (s < 0)
    {
        f = 0x001000000000ULL - (((0x001000000000ULL - f) * 186065280ULL) >> 28);
    }
    else
    {
        f = (f * 186065279ULL) >> 28;
    }

    // Determine sign (should only be possible for inputs <1)
    bool sign = (f & 0x000800000000ULL) != 0;
    if (sign)
    {
        f = 0x001000000000ULL - f;  // take absolute value
    }

    if (f < 0x40000000ULL)
    {
        // Result magnitude <1 ⇒ encode using posit regime for [-1,1)
        if (f != 0)
        {
            s = 34;
            while ((f & 0x20000000ULL) == 0)
            {
                f <<= 1;
                s += 1;
            }
            f = (f ^ 0x60000000ULL) | (((uint64_t) (1 ^ (s & 1)) << 29));
            s >>= 1;
            uint64_t bit = 1ULL << (s - 1);
            if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
            {
                f += bit;  // round to nearest-even
            }
            f >>= s;
        }
    }
    else
    {
        // Larger magnitude – encode the integer part in the regime field
        s = 0;
        while (f > 0x7FFFFFFFULL)
        {
            f = (f & 1ULL) | (f >> 1);
            s += 1;
        }
        f &= 0x3FFFFFFFULL;
        if (s & 1)
        {
            f |= 0x40000000ULL;
        }
        s >>= 1;
        f |= ((0x000200000000ULL << s) - 0x000100000000ULL);
        uint64_t bit = 0x00020000ULL << s;
        if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0)))
        {
            f += bit;
        }
        f >>= (s + 18);
    }

    if (sign)
    {
        f = 0x00010000ULL - f;  // restore sign
    }

    uA.ui = (uint_fast16_t) f;
    return uA.p;
}