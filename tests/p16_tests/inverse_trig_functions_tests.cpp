#include <cmath>
#include <random>

#include "common_tests/test_utils.h"

// Test for p16_acos_pi function implementation
TEST(Posit16MathFunctions, AcosPiFunction)
{
    // Create a distribution that generates values in the valid range for acos
    // [-1, 1]
    std::uniform_real_distribution<double> valid_dist(-0.99, 0.99);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = valid_dist(gen);
        posit16 p_a = posit16(f_a);

        // Calculate acos(x)/π using both methods
        posit16 p_result   = p_a.acos_pi();
        double  f_result   = std::acos(f_a) / M_PI;
        posit16 p_expected = posit16(f_result);

        // Allow small differences due to rounding
        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed acos_pi: acos(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
            << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value
            << " -> 0x" << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for acos_pi function
TEST(Posit16MathFunctions, AcosPiSpecificValues)
{
    // acos(1)/π = 0
    posit16 one        = posit16(1.0);
    posit16 zero       = posit16(0.0);
    posit16 result_one = one.acos_pi();
    ASSERT_EQ(result_one.value, zero.value)
        << "acos(1)/π = " << result_one.toDouble() << " but expected 0.0";

    // acos(0)/π = 0.5
    posit16 p_zero      = posit16(0.0);
    posit16 half        = posit16(0.5);
    posit16 result_zero = p_zero.acos_pi();
    ASSERT_EQ(result_zero.value, half.value)
        << "acos(0)/π = " << result_zero.toDouble() << " but expected 0.5";

    // acos(-1)/π = 1
    posit16 neg_one        = posit16(-1.0);
    posit16 p_one          = posit16(1.0);
    posit16 result_neg_one = neg_one.acos_pi();
    ASSERT_EQ(result_neg_one.value, p_one.value)
        << "acos(-1)/π = " << result_neg_one.toDouble() << " but expected 1.0";

    // Input outside [-1, 1] should result in NaR
    posit16 outside_range  = posit16(2.0);
    posit16 result_outside = outside_range.acos_pi();
    ASSERT_TRUE(result_outside.isNaR())
        << "acos(2)/π = " << result_outside.toDouble() << " but expected NaR";

    // NaR input should result in NaR
    posit16 p_nar;
    p_nar.toNaR();
    posit16 result_nar = p_nar.acos_pi();
    ASSERT_TRUE(result_nar.isNaR())
        << "acos_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p16_asin_pi function implementation
TEST(Posit16MathFunctions, AsinPiFunction)
{
    std::uniform_real_distribution<double> valid_dist(-0.99, 0.99);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = valid_dist(gen);
        posit16 p_a = posit16(f_a);

        posit16 p_result   = p_a.asin_pi();
        double  f_result   = std::asin(f_a) / M_PI;
        posit16 p_expected = posit16(f_result);

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed asin_pi: asin(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
            << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value
            << " -> 0x" << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for asin_pi function
TEST(Posit16MathFunctions, AsinPiSpecificValues)
{
    posit16 zero        = posit16(0.0);
    posit16 result_zero = zero.asin_pi();
    ASSERT_EQ(result_zero.value, zero.value)
        << "asin(0)/π = " << result_zero.toDouble() << " but expected 0.0";

    posit16 one        = posit16(1.0);
    posit16 half       = posit16(0.5);
    posit16 result_one = one.asin_pi();
    ASSERT_EQ(result_one.value, half.value)
        << "asin(1)/π = " << result_one.toDouble() << " but expected 0.5";

    posit16 neg_one        = posit16(-1.0);
    posit16 neg_half       = posit16(-0.5);
    posit16 result_neg_one = neg_one.asin_pi();
    ASSERT_EQ(result_neg_one.value, neg_half.value)
        << "asin(-1)/π = " << result_neg_one.toDouble() << " but expected -0.5";

    posit16 outside_range  = posit16(2.0);
    posit16 result_outside = outside_range.asin_pi();
    ASSERT_TRUE(result_outside.isNaR())
        << "asin(2)/π = " << result_outside.toDouble() << " but expected NaR";

    posit16 p_nar;
    p_nar.toNaR();
    posit16 result_nar = p_nar.asin_pi();
    ASSERT_TRUE(result_nar.isNaR())
        << "asin_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p16_atan_pi function implementation
TEST(Posit16MathFunctions, AtanPiFunction)
{
    std::uniform_real_distribution<double> safe_dist(-10.0, 10.0);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = safe_dist(gen);
        posit16 p_a = posit16(f_a);

        posit16 p_result   = p_a.atan_pi();
        double  f_result   = std::atan(f_a) / M_PI;
        posit16 p_expected = posit16(f_result);

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed atan_pi: atan(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
            << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value
            << " -> 0x" << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for atan_pi function
TEST(Posit16MathFunctions, AtanPiSpecificValues)
{
    posit16 zero        = posit16(0.0);
    posit16 result_zero = zero.atan_pi();
    ASSERT_EQ(result_zero.value, zero.value)
        << "atan(0)/π = " << result_zero.toDouble() << " but expected 0.0";

    posit16 one        = posit16(1.0);
    posit16 quarter    = posit16(0.25);
    posit16 result_one = one.atan_pi();
    ASSERT_EQ(result_one.value, quarter.value)
        << "atan(1)/π = " << result_one.toDouble() << " but expected 0.25";

    posit16 neg_one        = posit16(-1.0);
    posit16 neg_quarter    = posit16(-0.25);
    posit16 result_neg_one = neg_one.atan_pi();
    ASSERT_EQ(result_neg_one.value, neg_quarter.value)
        << "atan(-1)/π = " << result_neg_one.toDouble() << " but expected -0.25";

    posit16 p_nar;
    p_nar.toNaR();
    posit16 result_nar = p_nar.atan_pi();
    ASSERT_TRUE(result_nar.isNaR())
        << "atan_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}