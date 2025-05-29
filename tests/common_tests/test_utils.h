#ifndef SOFTPOSIT_COMMON_TEST_UTILS_H
#define SOFTPOSIT_COMMON_TEST_UTILS_H

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>  // Required for getenv
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "softposit_cpp.h"

// Define constants for number of tests
#ifdef NDEBUG
// Release mode values
constexpr int NTESTS8  = 1000000;
constexpr int NTESTS16 = 1000000;
constexpr int NTESTS32 = 1000000;
#else
// Debug mode values
constexpr int NTESTS8  = 10000;
constexpr int NTESTS16 = 10000;
constexpr int NTESTS32 = 10000;
#endif

// Function to get number of tests for P16, potentially reduced for CI on Ubuntu
inline int get_ntests16()
{
#ifdef REDUCE_P16_TESTS_ON_UBUNTU
    // This macro is expected to be defined by CMake for Ubuntu CI builds
#ifdef NDEBUG
    return 10000;  // Further reduced for Ubuntu CI Release
#else
    return 1000;  // Further reduced for Ubuntu CI Debug
#endif
#else
    // Standard number of tests for other builds or if macro is not defined
#ifdef NDEBUG
    return NTESTS16;  // Standard Release
#else
    return NTESTS16;  // Standard Debug
#endif
#endif
}

// Random generator setup - global to be used across all test files
extern std::mt19937 gen;

// Random distributions for p8 tests
extern std::uniform_int_distribution<int8_t> int_dist8;

// Random distributions for p16 tests
extern std::uniform_int_distribution<int16_t> int_dist16;

// Random distributions for p32 tests
extern std::uniform_int_distribution<int32_t> int_dist32;

// Compare doubles with epsilon tolerance
bool double_eq(double a, double b, double epsilon_max = 1e-5);

// Templated generic exact test function for posit operations
template <typename PositType>
void test_exact(int                                            num_tests,
                std::function<PositType(PositType, PositType)> posit_op,
                std::function<double(double, double)>          double_op,
                std::function<int(void)>                       random_generator)
{
    for (int i = 0; i < num_tests; i++)
    {
        // Generate random posit values
        auto raw_a = random_generator();
        auto raw_b = random_generator();

        // Create posit values
        PositType p_a, p_b;
        p_a.value = raw_a;
        p_b.value = raw_b;

        // Convert to double
        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();

        // Handle NaR (Not a Real) cases in posit
        if (p_a.isNaR() || p_b.isNaR() || std::isnan(f_a) || std::isnan(f_b))
        {
            // If either input is NaR/NaN, skip this test case
            continue;
        }

        // Calculate results
        PositType result          = posit_op(p_a, p_b);
        double    expected_double = double_op(f_a, f_b);
        PositType expected        = expected_double;

        // Compare results
        ASSERT_TRUE(double_eq(result.toDouble(), expected.toDouble()))
            << "Input: (" << f_a << ", " << f_b << ")"
            << "\nResult: " << result.toDouble() << "\nExpected: " << expected_double;
    }
}

// Templated ULP function (Unit in Last Place)
template <typename PositType>
auto ulp(PositType x, PositType y) -> decltype(x.value)
{
    // Cast to the appropriate type to avoid ambiguity with std::abs
    using ValueType = decltype(x.value);
    return static_cast<ValueType>(
        std::abs(static_cast<int64_t>(x.value) - static_cast<int64_t>(y.value)));
}

#endif  // SOFTPOSIT_COMMON_TEST_UTILS_H