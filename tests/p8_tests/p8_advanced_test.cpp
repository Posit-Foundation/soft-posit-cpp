// tests/p8_tests/p8_advanced_test.cpp
#include <gtest/gtest.h>
#include <cmath>
#include "softposit_cpp.h"
#include "../common_tests/test_helpers.h"
#include "../common_tests/test_constants.h"
#include "../common_tests/random_generators.h"

using namespace softposit_test;

//========================================================================================
// POSIT8 ADVANCED OPERATIONS TESTS
//========================================================================================

// Test square root operation
TEST(Posit8Advanced, Sqrt)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit value (ensure it's positive)
        posit8 p_a = RandomGenerators::randomPositivePosit8();
        double f_a = p_a.toDouble();

        posit8 p_result = p_a;
        p_result.sqrt();

        double f_result = std::sqrt(f_a);
        posit8 expected = f_result;

        ASSERT_EQ(p_result.value, expected.value)
            << "sqrt(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test rounding operation
TEST(Posit8Advanced, Round)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit value
        posit8 p_a = RandomGenerators::randomPosit8();
        double f_a = p_a.toDouble();

        posit8 p_result = p_a;
        p_result.rint();

        double f_result = std::round(f_a);
        posit8 expected = f_result;

        // Skip values at exactly 0.5 boundary
        if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10)
        {
            continue;
        }

        ASSERT_EQ(p_result.value, expected.value)
            << "round(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test fused multiply-add operation
TEST(Posit8Advanced, FusedMultiplyAdd)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit values
        posit8 p_a = RandomGenerators::randomPosit8();
        posit8 p_b = RandomGenerators::randomPosit8();
        posit8 p_c = RandomGenerators::randomPosit8();

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        posit8 p_result = p_a.fma(p_b, p_c);
        double f_result = std::fma(f_a, f_b, f_c);
        posit8 expected = f_result;

        // Allow 1 ULP difference for higher precision
        ASSERT_LE(ulp_p8(p_result, expected), 1)
            << "fma(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test edge cases for advanced operations
TEST(Posit8Advanced, EdgeCases)
{
    // Test sqrt of zero
    posit8 zero(0.0);
    posit8 sqrt_zero = zero;
    sqrt_zero.sqrt();
    EXPECT_EQ(sqrt_zero.value, zero.value);

    // Test sqrt of negative (should return NaR)
    posit8 negative(-1.0);
    posit8 sqrt_negative = negative;
    sqrt_negative.sqrt();
    EXPECT_TRUE(sqrt_negative.isNaR());

    // Test rounding of 0.0
    posit8 round_zero = zero;
    round_zero.rint();
    EXPECT_EQ(round_zero.value, zero.value);

    // Test FMA with zeros
    EXPECT_EQ(zero.fma(zero, zero).value, zero.value);

    // Test FMA with NAR
    posit8 nar;
    nar.toNaR();
    EXPECT_TRUE(nar.fma(zero, zero).isNaR());
    EXPECT_TRUE(zero.fma(nar, zero).isNaR());
    EXPECT_TRUE(zero.fma(zero, nar).isNaR());
}