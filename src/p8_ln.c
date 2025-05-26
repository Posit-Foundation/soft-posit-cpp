#include "internals.h"
#include "platform.h"
#include <math.h>
#include "p8_LUTs/p8_ln_LUT.h"


posit8_t p8_ln(posit8_t pA) {
  union ui8_p8 uA;
  uA.p = pA;

  // Check if input is negative or NaR (value > 127)
  if (uA.ui > 127) {
    // Return NaR for negative values or NaR
    uA.ui = 0x80; // NaR value
    return uA.p;
  }

  // Look up the result in the table
  union ui8_p8 uZ;
  uZ.ui = LOG8[uA.ui];
  return uZ.p;
}