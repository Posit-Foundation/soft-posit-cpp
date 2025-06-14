#include "test_utils.h"

// Initialize global random generator
std::random_device rd;
std::mt19937       gen(rd());

// Random distributions for p8 tests
std::uniform_int_distribution<int8_t> int_dist8(INT8_MIN, INT8_MAX);

// Random distributions for p16 tests
std::uniform_int_distribution<int16_t> int_dist16(INT16_MIN, INT16_MAX);

// Random distributions for p32 tests
std::uniform_int_distribution<int32_t> int_dist32(INT32_MIN, INT32_MAX);

// Compare doubles with epsilon tolerance
bool double_eq(double a, double b, double epsilon_max)
{
    // Special case: If both values are NaN, consider them equal
    if (std::isnan(a) && std::isnan(b))
    {
        return true;
    }

    // If only one value is NaN, they are not equal
    if (std::isnan(a) || std::isnan(b))
    {
        return false;
    }

    // Absolute difference check
    if (std::abs(a - b) < epsilon_max)
    {
        return true;
    }

    // Relative difference check (with small denominator guard)
    double rel_diff = std::abs(a - b) / (std::abs(a) + std::abs(b) + 1e-10);
    if (rel_diff < epsilon_max)
    {
        return true;
    }

    return false;
}