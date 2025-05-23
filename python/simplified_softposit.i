/*
 * Simplified SWIG interface file for SoftPosit
 * Only includes p8, p16, p32, and basic posit_2 functionality
 */

% module softposit %
    {
#include "../include/softposit.h"
#include "../include/softposit_types.h"
        % }

    /* Only expose the basic posit types and operations */
    % include<stdint.i>

    /* Posit type definitions from softposit_types.h */
    typedef struct {
  uint8_t v;
} posit8_t;
typedef struct {
  uint16_t v;
} posit16_t;
typedef struct {
  uint32_t v;
} posit32_t;
typedef struct {
  uint32_t v;
} posit_2_t;
typedef struct {
  uint32_t v;
} quire8_t;
typedef struct {
  uint64_t v[2];
} quire16_t;
typedef struct {
  uint64_t v[8];
} quire32_t;
typedef struct {
  uint64_t v[8];
} quire_2_t;

/* Basic conversion functions */
% rename(i64_to_p8) i64_to_p8;
posit8_t i64_to_p8(int64_t);
% rename(i64_to_p16) i64_to_p16;
posit16_t i64_to_p16(int64_t);
% rename(i64_to_p32) i64_to_p32;
posit32_t i64_to_p32(int64_t);
% rename(i64_to_pX2) i64_to_pX2;
posit_2_t i64_to_pX2(int64_t, int);

% rename(p8_to_i64) p8_to_i64;
int64_t p8_to_i64(posit8_t);
% rename(p16_to_i64) p16_to_i64;
int64_t p16_to_i64(posit16_t);
% rename(p32_to_i64) p32_to_i64;
int64_t p32_to_i64(posit32_t);
% rename(pX2_to_i64) pX2_to_i64;
int64_t pX2_to_i64(posit_2_t);

/* Posit to posit conversions */
% rename(p8_to_p16) p8_to_p16;
posit16_t p8_to_p16(posit8_t);
% rename(p8_to_p32) p8_to_p32;
posit32_t p8_to_p32(posit8_t);
% rename(p8_to_pX2) p8_to_pX2;
posit_2_t p8_to_pX2(posit8_t, int);

% rename(p16_to_p8) p16_to_p8;
posit8_t p16_to_p8(posit16_t);
% rename(p16_to_p32) p16_to_p32;
posit32_t p16_to_p32(posit16_t);
% rename(p16_to_pX2) p16_to_pX2;
posit_2_t p16_to_pX2(posit16_t, int);

% rename(p32_to_p8) p32_to_p8;
posit8_t p32_to_p8(posit32_t);
% rename(p32_to_p16) p32_to_p16;
posit16_t p32_to_p16(posit32_t);
% rename(p32_to_pX2) p32_to_pX2;
posit_2_t p32_to_pX2(posit32_t, int);

% rename(pX2_to_p8) pX2_to_p8;
posit8_t pX2_to_p8(posit_2_t);
% rename(pX2_to_p16) pX2_to_p16;
posit16_t pX2_to_p16(posit_2_t);
% rename(pX2_to_p32) pX2_to_p32;
posit32_t pX2_to_p32(posit_2_t);
% rename(pX2_to_pX2) pX2_to_pX2;
posit_2_t pX2_to_pX2(posit_2_t, int);

/* Integer functions */
% rename(p8_int) p8_int;
int p8_int(posit8_t);
% rename(p16_int) p16_int;
int p16_int(posit16_t);
% rename(p32_int) p32_int;
int p32_int(posit32_t);
% rename(pX2_int) pX2_int;
int pX2_int(posit_2_t);

/* Arithmetic operations */
% rename(p8_add) p8_add;
posit8_t p8_add(posit8_t, posit8_t);
% rename(p8_sub) p8_sub;
posit8_t p8_sub(posit8_t, posit8_t);
% rename(p8_mul) p8_mul;
posit8_t p8_mul(posit8_t, posit8_t);
% rename(p8_div) p8_div;
posit8_t p8_div(posit8_t, posit8_t);
% rename(p8_sqrt) p8_sqrt;
posit8_t p8_sqrt(posit8_t);
% rename(p8_mulAdd) p8_mulAdd;
posit8_t p8_mulAdd(posit8_t, posit8_t, posit8_t);

% rename(p16_add) p16_add;
posit16_t p16_add(posit16_t, posit16_t);
% rename(p16_sub) p16_sub;
posit16_t p16_sub(posit16_t, posit16_t);
% rename(p16_mul) p16_mul;
posit16_t p16_mul(posit16_t, posit16_t);
% rename(p16_div) p16_div;
posit16_t p16_div(posit16_t, posit16_t);
% rename(p16_sqrt) p16_sqrt;
posit16_t p16_sqrt(posit16_t);
% rename(p16_mulAdd) p16_mulAdd;
posit16_t p16_mulAdd(posit16_t, posit16_t, posit16_t);

% rename(p32_add) p32_add;
posit32_t p32_add(posit32_t, posit32_t);
% rename(p32_sub) p32_sub;
posit32_t p32_sub(posit32_t, posit32_t);
% rename(p32_mul) p32_mul;
posit32_t p32_mul(posit32_t, posit32_t);
% rename(p32_div) p32_div;
posit32_t p32_div(posit32_t, posit32_t);
% rename(p32_sqrt) p32_sqrt;
posit32_t p32_sqrt(posit32_t);
% rename(p32_mulAdd) p32_mulAdd;
posit32_t p32_mulAdd(posit32_t, posit32_t, posit32_t);

% rename(pX2_add) pX2_add;
posit_2_t pX2_add(posit_2_t, posit_2_t, int);
% rename(pX2_sub) pX2_sub;
posit_2_t pX2_sub(posit_2_t, posit_2_t, int);
% rename(pX2_mul) pX2_mul;
posit_2_t pX2_mul(posit_2_t, posit_2_t, int);
% rename(pX2_div) pX2_div;
posit_2_t pX2_div(posit_2_t, posit_2_t, int);
% rename(pX2_sqrt) pX2_sqrt;
posit_2_t pX2_sqrt(posit_2_t, int);
% rename(pX2_mulAdd) pX2_mulAdd;
posit_2_t pX2_mulAdd(posit_2_t, posit_2_t, posit_2_t, int);

/* Comparison operations */
% rename(p8_eq) p8_eq;
bool p8_eq(posit8_t, posit8_t);
% rename(p8_lt) p8_lt;
bool p8_lt(posit8_t, posit8_t);
% rename(p8_le) p8_le;
bool p8_le(posit8_t, posit8_t);

% rename(p16_eq) p16_eq;
bool p16_eq(posit16_t, posit16_t);
% rename(p16_lt) p16_lt;
bool p16_lt(posit16_t, posit16_t);
% rename(p16_le) p16_le;
bool p16_le(posit16_t, posit16_t);

% rename(p32_eq) p32_eq;
bool p32_eq(posit32_t, posit32_t);
% rename(p32_lt) p32_lt;
bool p32_lt(posit32_t, posit32_t);
% rename(p32_le) p32_le;
bool p32_le(posit32_t, posit32_t);

% rename(pX2_eq) pX2_eq;
bool pX2_eq(posit_2_t, posit_2_t);
% rename(pX2_lt) pX2_lt;
bool pX2_lt(posit_2_t, posit_2_t);
% rename(pX2_le) pX2_le;
bool pX2_le(posit_2_t, posit_2_t);

/* Double conversions */
% rename(convertDoubleToP8) convertDoubleToP8;
posit8_t convertDoubleToP8(double);
% rename(convertP8ToDouble) convertP8ToDouble;
double convertP8ToDouble(posit8_t);

% rename(convertDoubleToP16) convertDoubleToP16;
posit16_t convertDoubleToP16(double);
% rename(convertP16ToDouble) convertP16ToDouble;
double convertP16ToDouble(posit16_t);

% rename(convertDoubleToP32) convertDoubleToP32;
posit32_t convertDoubleToP32(double);
% rename(convertP32ToDouble) convertP32ToDouble;
double convertP32ToDouble(posit32_t);

% rename(convertDoubleToPX2) convertDoubleToPX2;
posit_2_t convertDoubleToPX2(double, int);
% rename(convertPX2ToDouble) convertPX2ToDouble;
double convertPX2ToDouble(posit_2_t);

/* Quire operations */
% rename(q8Clr) q8Clr;
quire8_t q8Clr();
% rename(q8_fdp_add) q8_fdp_add;
quire8_t q8_fdp_add(quire8_t, posit8_t, posit8_t);
% rename(q8_fdp_sub) q8_fdp_sub;
quire8_t q8_fdp_sub(quire8_t, posit8_t, posit8_t);
% rename(q8_to_p8) q8_to_p8;
posit8_t q8_to_p8(quire8_t);

% rename(q16Clr) q16Clr;
quire16_t q16Clr();
% rename(q16_fdp_add) q16_fdp_add;
quire16_t q16_fdp_add(quire16_t, posit16_t, posit16_t);
% rename(q16_fdp_sub) q16_fdp_sub;
quire16_t q16_fdp_sub(quire16_t, posit16_t, posit16_t);
% rename(q16_to_p16) q16_to_p16;
posit16_t q16_to_p16(quire16_t);

% rename(q32Clr) q32Clr;
quire32_t q32Clr();
% rename(q32_fdp_add) q32_fdp_add;
quire32_t q32_fdp_add(quire32_t, posit32_t, posit32_t);
% rename(q32_fdp_sub) q32_fdp_sub;
quire32_t q32_fdp_sub(quire32_t, posit32_t, posit32_t);
% rename(q32_to_p32) q32_to_p32;
posit32_t q32_to_p32(quire32_t);

% rename(qX2Clr) qX2Clr;
quire_2_t qX2Clr();
% rename(qX2_fdp_add) qX2_fdp_add;
quire_2_t qX2_fdp_add(quire_2_t, posit_2_t, posit_2_t);
% rename(qX2_fdp_sub) qX2_fdp_sub;
quire_2_t qX2_fdp_sub(quire_2_t, posit_2_t, posit_2_t);
% rename(qX2_to_pX2) qX2_to_pX2;
posit_2_t qX2_to_pX2(quire_2_t, int);

/* Define posit type extensions */
% extend posit8_t{posit8_t init(){posit8_t a;
a.v = 0;
return a;
}
void fromBits(int bits) { $self->v = bits & 0xFF; }
void toBits() { /* Call C function to print binary */ }
void toHex() { /* Call C function to print hex */ }
int toInt() { return $self->v; }
posit8_t __rshift__(int n) {
  posit8_t a;
  a.v = ($self->v >> n);
  return a;
}
posit8_t __lshift__(int n) {
  posit8_t a;
  a.v = ($self->v << n) & 0xFF;
  return a;
}
posit8_t __invert__() {
  $self->v = ~($self->v) & 0xFF;
  return *self;
}
posit8_t __neg__() {
  posit8_t a;
  a.v = -($self->v) & 0xFF;
  return a;
}
posit8_t __abs__() {
  posit8_t a;
  if (($self->v) >> 7)
    a.v = -($self->v) & 0xFF;
  else
    a.v = $self->v;
  return a;
}
posit8_t __and__(posit8_t other) {
  posit8_t a;
  a.v = $self->v & other.v;
  return a;
}
posit8_t __xor__(posit8_t other) {
  posit8_t a;
  a.v = $self->v ^ other.v;
  return a;
}
posit8_t __or__(posit8_t other) {
  posit8_t a;
  a.v = $self->v | other.v;
  return a;
}
bool isNaR() { return $self->v == (uint8_t)0x80; }
posit8_t toNaR() {
  $self->v = (uint8_t)0x80;
  return *$self;
}
}
;

/* Add similar extensions for posit16_t, posit32_t, and posit_2_t */

% pythonbegin %
    {from __future__ import absolute_import, print_function, division % }

    % pythoncode % {
  class posit8 : def __init__(self, value = None, bits = None) : try:
    if bits
      is not None : if isinstance (bits, (int))
          : self.v = posit8_t() self.v.v =
          bits & 0xFF else
          : raise Exception("Bits can only be set with integer values") else
          : if value is None : self.v = posit8_t() self.v.v =
              0 elif isinstance(value, (int))
          : self.v = _softposit.i64_to_p8(value) else : self.v =
                  _softposit.convertDoubleToP8(value) except Exception as error
          : print(repr(error))

                def __add__(self, other)
          : try : a
        = posit8(0) if isinstance (other, (int))
            : a.v = _softposit
                        .p8_add(self.v, _softposit.i64_to_p8(other)) elif
                        isinstance(other, (float))
            : a.v =
            _softposit
                .p8_add(self.v, _softposit.convertDoubleToP8(other)) else
            : a.v = _softposit.p8_add(self.v,
                                      other.v) return a except Exception as e
            : print(f "Error: {e}")

                  def __sub__(self, other)
            : try : a
          = posit8(0) if isinstance (other, (int))
              : a.v = _softposit
                          .p8_sub(self.v, _softposit.i64_to_p8(other)) elif
                          isinstance(other, (float))
              : a.v =
              _softposit
                  .p8_sub(self.v, _softposit.convertDoubleToP8(other)) else
              : a.v = _softposit.p8_sub(self.v,
                                        other.v) return a except Exception as e
              : print(f "Error: {e}")

                    def __mul__(self, other)
              : try : a
            = posit8(0) if isinstance (other, (int))
                : a.v = _softposit
                            .p8_mul(self.v, _softposit.i64_to_p8(other)) elif
                            isinstance(other, (float))
                : a.v =
                _softposit
                    .p8_mul(self.v, _softposit.convertDoubleToP8(other)) else
                : a.v = _softposit.p8_mul(
                    self.v, other.v) return a except Exception as e
                : print(f "Error: {e}")

                      def __truediv__(self, other)
                : try : a
              = posit8(0) if isinstance (other, (int))
                  : a.v = _softposit
                              .p8_div(self.v, _softposit.i64_to_p8(other)) elif
                              isinstance(other, (float))
                  : a.v =
                  _softposit
                      .p8_div(self.v, _softposit.convertDoubleToP8(other)) else
                  : a.v = _softposit.p8_div(
                      self.v, other.v) return a except Exception as e
                  : print(f "Error: {e}")

                        def __repr__(self)
                  : a = float(_softposit.convertP8ToDouble(
                            self.v)) if (a == float('inf')) or
                        (a == float('-inf')) or
                        (a == float('nan'))
                  : return "NaR" else : return str(a)

                                            def __str__(self)
                  : a = float(_softposit.convertP8ToDouble(
                            self.v)) if (a == float('inf')) or
                        (a == float('-inf')) or
                        (a == float('nan'))
                  : return "NaR" else : return str(a)

                                            def __float__(self)
                  : return float(_softposit.convertP8ToDouble(self.v))

                        def __int__(self)
                  : return _softposit.p8_int(self.v)

                        class posit16
                  : def __init__(self, value = None, bits = None)
                  : try:
                if bits
                  is not None : if isinstance (bits, (int))
                      : self.v = posit16_t() self.v.v =
                      bits &
                      0xFFFF else
                      : raise Exception(
                            "Bits can only be set with integer values") else
                      : if value is None : self.v = posit16_t() self.v.v =
                          0 elif isinstance(value, (int))
                      : self.v = _softposit.i64_to_p16(value) else : self.v =
                              _softposit.convertDoubleToP16(value)
                                  except Exception as error
                      : print(repr(error))

                            def __add__(self, other)
                      : try : a
                    = posit16(0) if isinstance (other, (int))
                        : a.v = _softposit
                                    .p16_add(self.v,
                                             _softposit.i64_to_p16(other)) elif
                                    isinstance(other, (float))
                        : a.v =
                        _softposit
                            .p16_add(self.v,
                                     _softposit.convertDoubleToP16(other)) else
                        : a.v = _softposit.p16_add(
                            self.v, other.v) return a except Exception as e
                        : print(f "Error: {e}")

                              def __sub__(self, other)
                        : try : a
                      = posit16(0) if isinstance (other, (int))
                          : a.v =
                          _softposit
                              .p16_sub(self.v, _softposit.i64_to_p16(other))
                                  elif
                                  isinstance(other, (float))
                          : a.v = _softposit
                                      .p16_sub(self.v,
                                               _softposit.convertDoubleToP16(
                                                   other)) else
                          : a.v = _softposit.p16_sub(
                              self.v, other.v) return a except Exception as e
                          : print(f "Error: {e}")

                                def __mul__(self, other)
                          : try : a
                        = posit16(0) if isinstance (other, (int))
                            : a.v =
                            _softposit
                                .p16_mul(self.v, _softposit.i64_to_p16(other))
                                    elif
                                    isinstance(other, (float))
                            : a.v = _softposit
                                        .p16_mul(self.v,
                                                 _softposit.convertDoubleToP16(
                                                     other)) else
                            : a.v = _softposit.p16_mul(
                                self.v, other.v) return a except Exception as e
                            : print(f "Error: {e}")

                                  def __truediv__(self, other)
                            : try : a
                          = posit16(0) if isinstance (other, (int))
                              : a.v =
                              _softposit
                                  .p16_div(self.v, _softposit.i64_to_p16(other))
                                      elif
                                      isinstance(other, (float))
                              : a.v =
                                  _softposit
                                      .p16_div(self.v,
                                               _softposit.convertDoubleToP16(
                                                   other)) else
                              : a.v = _softposit.p16_div(
                                      self.v,
                                      other.v) return a except Exception as e
                              : print(f "Error: {e}")

                                    def __repr__(self)
                              : a = float(_softposit.convertP16ToDouble(
                                        self.v)) if (a == float('inf')) or
                                    (a == float('-inf')) or
                                    (a == float('nan'))
                              : return "NaR" else : return str(a)

                                                        def __str__(self)
                              : a = float(_softposit.convertP16ToDouble(
                                        self.v)) if (a == float('inf')) or
                                    (a == float('-inf')) or
                                    (a == float('nan'))
                              : return "NaR" else : return str(a)

                                                        def __float__(self)
                              : return float(
                                        _softposit.convertP16ToDouble(self.v))

                                    def __int__(self)
                              : return _softposit.p16_int(self.v)

                                    class posit32
                              : def __init__(self, value = None, bits = None)
                              : try:
                            if bits
                              is not None : if isinstance (bits, (int))
                                  : self.v = posit32_t() self.v.v =
                                  bits & 0xFFFFFFFF else
                                  : raise Exception("Bits can only be set with "
                                                    "integer values") else
                                  : if value is None
                                  : self.v = posit32_t()
                                                 self.v.v = 0 elif isinstance(
                                                                value, (int))
                                  : self.v =
                                      _softposit.i64_to_p32(value) else
                                  : self.v =
                                          _softposit.convertDoubleToP32(value)
                                              except Exception as error
                                  : print(repr(error))

                                        def __add__(self, other)
                                  : try : a
                                = posit32(0) if isinstance (other, (int))
                                    : a.v = _softposit
                                                .p32_add(self.v,
                                                         _softposit.i64_to_p32(
                                                             other)) elif
                                                isinstance(other, (float))
                                    : a.v =
                                    _softposit
                                        .p32_add(self.v,
                                                 _softposit.convertDoubleToP32(
                                                     other)) else
                                    : a.v = _softposit.p32_add(
                                        self.v,
                                        other.v) return a except Exception as e
                                    : print(f "Error: {e}")

                                          def __sub__(self, other)
                                    : try : a
                                  = posit32(0) if isinstance (other, (int))
                                      : a.v =
                                      _softposit
                                          .p32_sub(self.v,
                                                   _softposit.i64_to_p32(other))
                                              elif
                                              isinstance(other, (float))
                                      : a.v =
                                          _softposit
                                              .p32_sub(
                                                  self.v,
                                                  _softposit.convertDoubleToP32(
                                                      other)) else
                                      : a.v = _softposit.p32_sub(
                                              self.v, other.v) return a except
                                                  Exception as e
                                      : print(f "Error: {e}")

                                            def __mul__(self, other)
                                      : try : a
                                    = posit32(0) if isinstance (other, (int))
                                        : a.v = _softposit
                                                    .p32_mul(
                                                        self.v,
                                                        _softposit.i64_to_p32(
                                                            other)) elif
                                                    isinstance(other, (float))
                                        : a.v =
                                        _softposit
                                            .p32_mul(
                                                self.v,
                                                _softposit.convertDoubleToP32(
                                                    other)) else : a.v =
                                            _softposit.p32_mul(
                                                self.v, other.v) return a except
                                                Exception as e
                                        : print(f "Error: {e}")

                                              def __truediv__(self, other)
                                        : try : a
                                      = posit32(0) if isinstance (other, (int))
                                          : a.v = _softposit
                                                      .p32_div(
                                                          self.v,
                                                          _softposit.i64_to_p32(
                                                              other)) elif
                                                      isinstance(other, (float))
                                          : a.v = _softposit
                                                      .p32_div(
                                                          self.v,
                                                          _softposit
                                                              .convertDoubleToP32(
                                                                  other)) else
                                          : a.v = _softposit.p32_div(
                                          self.v, other.v) return a except
                                                      Exception as e
                                          : print(f "Error: {e}")

                                                def __repr__(self)
                                          : a =
                                          float(_softposit.convertP32ToDouble(
                                              self.v)) if (a == float('inf')) or
                                          (a == float('-inf')) or
                                          (a == float('nan'))
                                          : return "NaR" else
                                          : return str(a)

                                                def __str__(self)
                                          : a =
                                              float(
                                                  _softposit.convertP32ToDouble(
                                                      self.v)) if (a ==
                                                                   float(
                                                                       'inf')) or
                                              (a == float('-inf')) or
                                              (a == float('nan'))
                                          : return "NaR" else
                                          : return str(a)

                                                def __float__(self)
                                          : return float(
                                                  _softposit.convertP32ToDouble(
                                                      self.v))

                                                def __int__(self)
                                          : return _softposit.p32_int(self.v) %
}