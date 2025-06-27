#include <math.h>
#include <stdint.h>

#include "internals.h"
#include "platform.h"

// Polynomial used for cos_pi fixed-point approximation
static inline uint64_t p16_cos_pi_poly(uint64_t f)
{
    if (f < 0x000E6001ULL)
    {
        return 0x01FFFFFFULL;  // rounds up to 1.0
    }
    uint64_t fsq = f >> 11;  // convert to 17-bit fixed-point
    fsq          = (fsq * fsq) >> 8;
    uint64_t s   = 349194ULL - ((fsq * 28875ULL) >> 25);
    s            = 4255560ULL - ((fsq * s) >> 24);
    s            = 20698014ULL - ((fsq * s) >> 24);
    return 33554428ULL - ((fsq * s) >> 23);
}

posit16_t p16_cos_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    // Handle NaR directly
    if (uA.ui == 0x8000)
    {
        return pA;
    }

    uint64_t f = (uint64_t) uA.ui;

    // Work with absolute value
    if (f & 0x8000ULL)
    {
        f = 0x10000ULL - f;
    }

    // Decode posit to 28-bit fixed-point
    int32_t s;
    if (f != 0)
    {
        if (f & 0x4000ULL)
        {
            s = 16;
            while (f & 0x2000ULL)
            {
                f <<= 1;
                s += 2;
            }
        }
        else
        {
            s = 14;
            while ((f & 0x2000ULL) == 0)
            {
                f <<= 1;
                s -= 2;
            }
        }
        if (f & 0x1000ULL)
        {
            s += 1;
        }
        f = (f & 0x0FFFULL) | 0x1000ULL;  // restore hidden bit
        f = (s < 0) ? (f >> -s) : (f << s);
    }

    uint64_t quadrant = f >> 27;  // multiples of 1/2
    f &= 0x07FFFFFFULL;           // modulo 1/2

    uint64_t sign = (((quadrant + 1ULL) & 2ULL) != 0ULL) ? 0x8000ULL : 0ULL;

    if (f == 0)
    {
        // cos is ±1 or 0 depending on quadrant
        if (quadrant & 1ULL)
        {
            uA.ui = 0;  // zero
        }
        else
        {
            uA.ui = (uint16_t) (sign | 0x4000);
        }
        return uA.p;
    }

    if (quadrant & 1ULL)
    {
        f = 0x08000000ULL - f;
    }

    f = p16_cos_pi_poly(f);

    // Convert fixed-point back to posit16
    s = 1;
    while ((f & 0x01000000ULL) == 0)
    {
        f <<= 1;
        s += 1;
    }
    int bit = s & 1;
    if (bit == 0)
    {
        f &= 0x00FFFFFFULL;  // encode exponent bit
    }
    s = (s >> 1) + 12;
    if (bit == 0)
    {
        s -= 1;
    }

    f |= 0x02000000ULL;  // encode regime termination

    uint64_t round_bit = 1ULL << (s - 1);
    if ((f & round_bit) && (((f & (round_bit - 1)) != 0) || ((f & (round_bit << 1)) != 0)))
    {
        f += round_bit;
    }

    f >>= s;

    if (sign)
    {
        f = 0x10000ULL - f;
    }

    uA.ui = (uint16_t) f;
    return uA.p;
}