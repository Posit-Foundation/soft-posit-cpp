#include "internals.h"
#include "platform.h"

// Helper function for polynomial evaluation used in exponential function
static uint64_t poly(uint64_t f) {
  uint64_t s;

  s = (f * 7529) >> 26;
  s = (f * (20487 + s)) >> 20;
  s = (f * (0x004F8300 + s)) >> 24;
  s = (f * (0x038CC980 + s)) >> 20;
  s = (f * (0x0001EBFFC800 + s)) >> 26;
  return ((f * (0x0002C5C83600 + s)) >> 22) + 2048;
}

posit16_t p16_exp(posit16_t pA) {
  union ui16_p16 uA;
  uint_fast16_t uiA;
  uint64_t f;
  int32_t s;
  uint64_t bit;

  uA.p = pA;
  uiA = uA.ui;
  f = uiA;

  // Calculate the exponential for given posit pA
  if (uiA < 28846) {
    // result does not round up to maxpos
    if (uiA < 192) {
      // small positive values that round to 1
      uA.ui = 0x4000; // ONE value in posit16
      return uA.p;
    }

    if (f & 0x4000) {
      // decode regime
      s = 8;
      while (f & 0x2000) {
        f <<= 1;
        s += 2;
      }
    } else {
      s = 6;
      while ((f & 0x2000) == 0) {
        f <<= 1;
        s -= 2;
      }
    }

    if (f & 0x1000) {
      s += 1; // decode exponent
    }
    f = (f & 0x0FFF) | 0x1000; // decode fraction
    f = ((s < 0 ? f >> -s : f << s) * 48408813) >> 20;
    uint64_t s_floor = f >> 25; // s_floor now stores floor(x)
    f = poly(f & 0x01FFFFFF);   // 37 fraction bits of exp(x)
    bit = (s_floor & 1) << 37;  // exponent bit of exp(x)
    s_floor >>= 1;              // regime length of exp(x)
    f |= ((0x010000000000 << s_floor) - 0x008000000000) | bit;

    bit = 1ULL << (24 + s_floor); // location of bit n-plus-1
    if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0))) {
      f += bit;
    }

    uA.ui = (f >> (25 + s_floor)); // return rounded exp(x) as posit
    return uA.p;
  } else if (uiA > 36690) {
    // result does not round up to minpos
    if (uiA > 65407) {
      // small negative values that round to 1
      uA.ui = 0x4000; // ONE value in posit16
      return uA.p;
    }

    if (f & 0x4000) {
      // decode regime
      s = 7;
      while (f & 0x2000) {
        f <<= 1;
        s -= 2;
      }
    } else {
      s = 9;
      while ((f & 0x2000) == 0) {
        f <<= 1;
        s += 2;
      }
    }

    if (f & 0x1000) {
      s -= 1; // decode exponent
    }
    f = (f & 0x0FFF) | 0x01FFE000; // decode fraction

    if (s < 0) {
      f = (f >> -s) | (0x02000000 - (1 << (13 + s)));
    } else {
      f = (f << s) & 0x01FFFFFF;
    }

    f = (0x0004000000000000 - ((0x02000000 - f) * 48408813)) >> 20;

    int64_t s_floor = (f >> 25) - 32; // s_floor now stores floor(x)
    f = poly(f & 0x01FFFFFF);         // 37 fraction bits of exp(x)
    bit = (s_floor & 1) << 37;        // exponent bit of exp(x)
    uint64_t s_shift = (-1 - s_floor) >> 1;
    f |= 0x004000000000 | bit; // Install regime end bit

    bit = 1ULL << (24 + s_shift); // location of bit n-plus-1
    if ((f & bit) && (((f & (bit - 1)) != 0) || ((f & (bit << 1)) != 0))) {
      f += bit;
    }

    uA.ui = (f >> (25 + s_shift)); // return rounded exp(x) as posit
    return uA.p;
  }

  // Section for exception cases
  if (uiA < 0x8000) {
    uA.ui = 0x7FFF; // return maxpos
  } else if (uiA > 0x8000) {
    uA.ui = 0x0001; // return minpos
  } else {
    uA.ui = 0x8000; // return NaR
  }

  return uA.p;
}
