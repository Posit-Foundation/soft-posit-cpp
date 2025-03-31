
/*============================================================================

This C source file is part of the SoftPosit Posit Arithmetic Package
by S. H. Leong (Cerlane).

Copyright 2017, 2018 A*STAR.  All rights reserved.

This C source file was based on SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
        this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions, and the following disclaimer in the
documentation and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
        be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include "internals.h"
#include "platform.h"

uint_fast16_t uint16_with_sign(uint_fast16_t value, bool is_negative) {
  return is_negative ? -value & 0xFFFF : value;
}

void separate_bits_tmp(uint_fast16_t ui, int_fast8_t *k, uint_fast16_t *tmp) {
  bool regS = signregP16UI(ui);
  *tmp = (ui << 2) & 0xFFFF;
  *k = 0;

  if (regS) {
    while (*tmp >> 15) {
      (*k)++;
      *tmp = (*tmp << 1) & 0xFFFF;
    }
  } else {
    *k = -1;
    while (!(*tmp >> 15)) {
      (*k)--;
      *tmp = (*tmp << 1) & 0xFFFF;
    }
    *tmp &= 0x7FFF;
  }
}

void calculate_regime(int_fast8_t k, uint_fast16_t *regime, bool *regS,
                      uint_fast16_t *reg) {
  if (k < 0) {
    *reg = (-k & 0xFFFF);
    *regS = false;
    *regime = 0x4000 >> *reg;
  } else {
    *reg = k + 1;
    *regS = true;
    *regime = 0x7FFF - (0x7FFF >> *reg);
  }
}

posit16_t softposit_mulAddP16(uint_fast16_t uiA, uint_fast16_t uiB,
                              uint_fast16_t uiC, uint_fast16_t opcode) {
  union ui16_p16 uZ;
  uint_fast16_t regZ, fracZ, regime, tmp;
  bool signA, signB, signC, signZ, regSZ, bitNPlusOne = false, bitsMore = false,
                                          rcarry;
  int_fast8_t expA, expC, expZ;
  int_fast8_t kA = 0, kB = 0, kC = 0, kZ = 0;
  int_fast16_t shiftRight;
  uint_fast32_t frac32C = 0, frac32Z = 0;

  // Apply sign operations based on the op type
  if (opcode == softposit_mulAdd_subC)
    uiC = -uiC;
  if (opcode == softposit_mulAdd_subProd)
    uiA = -uiA;

  // NaR
  if (uiA == 0x8000 || uiB == 0x8000 || uiC == 0x8000) {
    uZ.ui = 0x8000;
    return uZ.p;
  } else if (uiA == 0 || uiB == 0) {
    uZ.ui = uiC;
    return uZ.p;
  }

  signA = signP16UI(uiA);
  signB = signP16UI(uiB);
  signC = signP16UI(uiC);
  signZ = signA ^ signB; // Base multiplication sign

  // Convert to absolute values for computation
  if (signA)
    uiA = -uiA & 0xFFFF;
  if (signB)
    uiB = -uiB & 0xFFFF;
  if (signC)
    uiC = -uiC & 0xFFFF;

  // Extract components for A
  separate_bits_tmp(uiA, &kA, &tmp);
  expA = tmp >> 14;
  uint_fast16_t fracA = 0x8000 | (tmp << 1); // Use first bit for hidden bit

  // Extract components for B
  separate_bits_tmp(uiB, &kB, &tmp);
  kA += kB; // Accumulate scales
  expA += tmp >> 14;
  frac32Z = (uint_fast32_t)fracA *
            (0x8000 | (tmp << 1)); // Multiply fractions with hidden bits

  // Handle exponent accumulation and carry
  if (expA > 1) {
    kA++;
    expA ^= 0x2;
  }

  rcarry = (frac32Z & 0x80000000) != 0; // 1st bit of frac32Z
  if (rcarry) {
    if (expA != 0)
      kA++;
    expA ^= 1;
    frac32Z >>= 1;
  }

  // Add/subtract third operand
  if (uiC != 0) {
    // Extract components for C
    separate_bits_tmp(uiC, &kC, &tmp);
    expC = tmp >> 14;
    frac32C = ((0x4000 | tmp) << 16);

    shiftRight = ((kA - kC) << 1) + (expA - expC); // Scale difference

    if (shiftRight < 0) { // |uiC| > |Prod Z|
      if (shiftRight <= -31) {
        bitsMore = true;
        frac32Z = 0;
      } else if (((frac32Z << (32 + shiftRight)) & 0xFFFFFFFF) != 0) {
        bitsMore = true;
      }

      if (signZ == signC) {
        frac32Z = frac32C + (frac32Z >> -shiftRight);
      } else { // different signs
        frac32Z = frac32C - (frac32Z >> -shiftRight);
        signZ = signC;
        if (bitsMore)
          frac32Z -= 1;
      }
      kZ = kC;
      expZ = expC;
    } else if (shiftRight > 0) { // |uiC| < |Prod|
      if (shiftRight >= 31) {
        bitsMore = true;
        frac32C = 0;
      } else if (((frac32C << (32 - shiftRight)) & 0xFFFFFFFF) != 0) {
        bitsMore = true;
      }

      if (signZ == signC) {
        frac32Z = frac32Z + (frac32C >> shiftRight);
      } else {
        frac32Z = frac32Z - (frac32C >> shiftRight);
        if (bitsMore)
          frac32Z -= 1;
      }
      kZ = kA;
      expZ = expA;
    } else { // Same scale
      if (frac32C == frac32Z &&
          signZ != signC) { // Check if same magnitude but opposite signs
        uZ.ui = 0;          // Result is zero
        return uZ.p;
      } else {
        if (signZ == signC) {
          frac32Z += frac32C;
        } else {
          if (frac32Z < frac32C) {
            frac32Z = frac32C - frac32Z;
            signZ = signC;
          } else {
            frac32Z -= frac32C;
          }
        }
      }
      kZ = kA; // Same scale
      expZ = expA;
    }

    // Normalize the result
    rcarry = (frac32Z & 0x80000000) != 0; // first left bit
    if (rcarry) {
      if (expZ != 0)
        kZ++;
      expZ ^= 1;
      if (frac32Z & 0x1)
        bitsMore = true;
      frac32Z = (frac32Z >> 1) & 0x7FFFFFFF;
    } else {
      // For subtract cases, renormalize if needed
      if (frac32Z != 0) {
        while ((frac32Z >> 29) == 0) {
          kZ--;
          frac32Z <<= 2;
        }
      }
      bool ecarry = ((frac32Z & 0x40000000) >> 30) != 0;
      if (!ecarry) {
        if (expZ == 0)
          kZ--;
        expZ ^= 1;
        frac32Z <<= 1;
      }
    }
  } else {
    kZ = kA;
    expZ = expA;
  }

  // Calculate regime and assemble the result
  calculate_regime(kZ, &regime, &regSZ, &regZ);

  if (regZ > 14) {
    // max or min pos. exp and frac does not matter.
    uZ.ui = regSZ ? 0x7FFF : 0x1;
  } else {
    // Remove hidden bits
    frac32Z &= 0x3FFFFFFF;
    fracZ = frac32Z >> (regZ + 17);

    if (regZ != 14) {
      bitNPlusOne = (frac32Z >> regZ) & 0x10000;
    } else if (frac32Z > 0) {
      fracZ = 0;
      bitsMore = true;
    }
    if (regZ == 14 && expZ != 0) {
      bitNPlusOne = true;
    }

    uZ.ui = packToP16UI(regime, regZ, expZ, fracZ);

    if (bitNPlusOne) {
      if ((frac32Z << (16 - regZ)) & 0xFFFFFFFF) {
        bitsMore = true;
      }
      uZ.ui += (uZ.ui & 1) | (bitsMore ? 1 : 0);
    }
  }

  // Apply sign
  if (signZ)
    uZ.ui = -uZ.ui & 0xFFFF;
  return uZ.p;
}