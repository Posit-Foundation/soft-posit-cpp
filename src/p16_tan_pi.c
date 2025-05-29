#include <math.h>
#include <stdint.h>

#include "internals.h"
#include "platform.h"

// Polynomial approximation for tan_pi
static inline uint64_t p16_tan_pi_poly(uint64_t f)
{
    if (f < 0xE001ULL)
    {
        return (f * 102943ULL) >> 15;  // linear approximation for small inputs
    }
    uint64_t fs  = f >> 9;
    uint64_t fsq = (fs * fs) >> 10;
    uint64_t num = (fsq * 182527ULL) >> 27;
    uint64_t den = (fsq * 13335493ULL) >> 25;
    num          = (fsq * (3648552ULL - num)) >> 23;
    den          = 0x08000000ULL - ((fsq * (295106440ULL - den)) >> 27);
    num          = (fs * (105414368ULL - num)) << 11;
    return num / den;
}

posit16_t p16_tan_pi(posit16_t pA)
{
    union ui16_p16 uA;
    uA.p = pA;

    uint64_t f    = (uint64_t) uA.ui;
    uint64_t sign = f & 0x8000ULL;
    if (sign)
    {
        f = 0x10000ULL - f;  // absolute value
    }

    // Handle integer inputs and NaR
    if (f > 31743ULL)
    {
        if (f == 0x8000ULL)
        {
            return pA;  // NaR propagates
        }
        else
        {
            uA.ui = 0;  // tanpi of integer is zero
            return uA.p;
        }
    }

    int32_t s = 0;
    if (f != 0)
    {
        // Decode posit to fixed-point
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
        f = (f & 0x0FFFULL) | 0x1000ULL;
        f = (s < 0) ? (f >> -s) : (f << s);
    }

    f &= 0x0FFFFFFFULL;  // keep 28-bit fraction (28 bits)

    if (f == 0)
    {
        uA.ui = 0;  // tanpi integer = 0
        return uA.p;
    }

    uint64_t quadrant = f >> 27;  // multiple of 1/2
    f &= 0x07FFFFFFULL;           // modulo 1/2

    if (f == 0)
    {
        // tan undefined at 1/2 * odd integers --> NaR
        uA.ui = 0x8000;
        return uA.p;
    }

    if (quadrant != 0)
    {
        sign ^= 0x8000ULL;  // flip sign for odd quadrants
    }

    if ((f & 0x03FFFFFFULL) == 0)
    {
        // tanpi is ±1 at quarter-odd integers
        uA.ui = (uint16_t) (sign | 0x4000);
        return uA.p;
    }

    if (sign)
    {
        f = 0x08000000ULL - f;  // reverse input
    }
    if (uA.ui & 0x8000)
    {  // original sign of input
        f = 0x08000000ULL - f;
    }

    f = p16_tan_pi_poly(f);

    if (f > 0x0FFFFFFFULL)
    {
        // large magnitude result, encode with larger exponent
        s = 12;
        while ((f & 0x010000000000ULL) == 0)
        {
            f <<= 1;
            s -= 1;
        }
        if ((s & 1) == 0)
        {
            f &= 0x00FFFFFFFFFFULL;
        }
        s = (s >> 1) + 28;
        f |= (2ULL << (14 + s)) - (1ULL << 42);
    }
    else
    {
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
            f &= 0x07FFFFFFULL;
        }
        f |= 0x10000000ULL;
    }

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