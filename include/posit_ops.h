#ifndef POSIT_OPS_H
#define POSIT_OPS_H

#include "softposit.h"
#include <iostream>

// Conversion functions for posit
inline posit32_t int_to_posit32(int value) {
    posit32_t p;
    p.v = value;
    return p;
}

inline posit32_t double_to_posit32(double value) {
    return convertDoubleToP32(value);
}

inline double posit32_to_double(posit32_t value) {
    return convertP32ToDouble(value);
}

// Define overloaded operators for posit32_t
inline posit32_t operator+(const posit32_t& a, const posit32_t& b) {
    return p32_add(a, b);
}

inline posit32_t operator-(const posit32_t& a, const posit32_t& b) {
    return p32_sub(a, b);
}

inline posit32_t operator*(const posit32_t& a, const posit32_t& b) {
    return p32_mul(a, b);
}

inline posit32_t operator/(const posit32_t& a, const posit32_t& b) {
    return p32_div(a, b);
}

inline bool operator==(const posit32_t& a, const posit32_t& b) {
    return a.v == b.v;
}

std::ostream& operator<<(std::ostream& os, const posit32_t& num) {
    os << posit32_to_double(num) << " (posit32_t)";
    return os;
}

#endif // POSIT_OPS_H