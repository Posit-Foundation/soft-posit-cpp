#include <math.h>
#include <stdint.h>

#include "internals.h"
#include "platform.h"

// Polynomial approximation used by sin_pi fixed-point path
static inline uint64_t p16_sin_pi_poly(uint64_t f)
{
    if (f < 0x000A5801ULL)
    {
        return (f * 102943ULL) >> 15;  // linear approximation suffices
    }
    uint64_t fs  = f >> 11;
    uint64_t fsq = (fs * fs) >> 8;
    uint64_t s   = (fsq * 650ULL) >> 25;
    s            = (fsq * (9813ULL - s)) >> 23;
    s            = (fsq * (334253ULL - s)) >> 23;
    s            = (fsq * (5418741ULL - s)) >> 22;
    return (fs * (52707180ULL - s)) >> 13;
}

posit16_t p16_sin_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint64_t f = (uint64_t) uA.ui;  // work in 64-bit for intermediate headroom

    uint64_t sign = f & 0x8000ULL;
    if (sign)
    {
        // Take two's complement to work with absolute value
        f = 0x10000ULL - f;
    }

    // Detect integer inputs and NaR
    if (f > 31743ULL)
    {  // 0x7BFF – largest posit16 less than 1.0
        if (f == 0x8000ULL)
        {
            // NaR propagates
            return pA;
        }
        else
        {
            // sin(pi * integer) == 0
            uA.ui = 0;
            return uA.p;
        }
    }

    if (f == 0)
    {
        // sinpi(0) == 0
        uA.ui = 0;
        return uA.p;
    }

    int32_t s;
    if (f & 0x4000ULL)
    {
        // Decode regime for numbers >= 1
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
        s += 1;  // decode exponent bit
    }

    // Restore hidden bit and produce 12-bit fraction
    f = (f & 0x0FFFULL) | 0x1000ULL;
    f = (s < 0) ? (f >> -s) : (f << s);

    f &= 0x1FFFFFFFULL;  // keep 29 bits (fixed-point with 28-bit fraction)

    uint64_t quadrant = f >> 27;  // multiple of 1/2
    f &= 0x07FFFFFFULL;           // modulo 1/2

    if (quadrant & 2ULL)
    {
        // Quadrants 2 and 3 flip the sign
        sign ^= 0x8000ULL;
    }

    if (f == 0)
    {
        if (quadrant & 1ULL)
        {
            uA.ui = (uint16_t) (sign | 0x4000);  // ±1
        }
        else
        {
            uA.ui = 0;  // 0
        }
        return uA.p;
    }

    if (quadrant & 1ULL)
    {
        // Mirror around 1/2
        f = 0x08000000ULL - f;
    }

    f = p16_sin_pi_poly(f);

    // Convert 28-bit fixed point back to posit16
    s = 1;
    while ((f & 0x08000000ULL) == 0)
    {
        f <<= 1;
        s += 1;
    }
    int bit = s & 1;
    s       = (s >> 1) + 14 + bit;
    if (bit == 0)
    {
        f &= 0x07FFFFFFULL;  // encode exponent bit
    }

    f |= 0x10000000ULL;  // encode regime termination bit

    uint64_t round_bit = 1ULL << (s - 1);
    if ((f & round_bit) && (((f & (round_bit - 1)) != 0) || ((f & (round_bit << 1)) != 0)))
    {
        // round to nearest, ties to even
        f += round_bit;
    }

    f >>= s;

    if (sign)
    {
        f = 0x10000ULL - f;  // re-apply original sign via two's complement
    }

    uA.ui = (uint16_t) f;
    return uA.p;
}