#include <cmath>
#include <random>

#include "common_tests/test_utils.h"

// Test for p16_exp function implementation
TEST(Posit16MathFunctions, ExpFunction)
{
    // Create a distribution that generates values across the full posit16 range
    std::uniform_int_distribution<uint16_t> full_dist(0, 0xFFFF);

    for (int i = 0; i < NTESTS16; i++)
    {
        posit16 p_a;
        p_a.value = full_dist(gen);

        // Skip NaR values for input
        if (p_a.isNaR())
        {
            continue;
        }

        // Calculate exponential using both methods
        double  f_a        = p_a.toDouble();
        posit16 p_result   = p_a.exp();
        double  f_result   = std::exp(f_a);
        posit16 p_expected = posit16(f_result);

        // Skip comparison if expected result is zero or NaR
        if (p_expected.value == 0 || p_expected.isNaR())
        {
            continue;
        }

        // Allow small differences due to rounding
        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed exp: exp(" << p_a.toDouble() << ") = " << p_result.toDouble()
            << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value
            << " -> 0x" << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for exponential function
TEST(Posit16MathFunctions, ExpSpecificValues)
{
    // exp(0) = 1
    posit16 zero        = posit16(0.0);
    posit16 one         = posit16(1.0);
    posit16 result_zero = zero.exp();
    ASSERT_EQ(result_zero.value, one.value)
        << "exp(0) = " << result_zero.toDouble() << " but expected 1.0";

    // exp(1) ≈ 2.718...
    posit16 p_one      = posit16(1.0);
    posit16 result_one = p_one.exp();
    posit16 expected_e = posit16(std::exp(1.0));
    ASSERT_EQ(result_one.value, expected_e.value)
        << "exp(1) = " << result_one.toDouble() << " but expected " << expected_e.toDouble();

    // exp(-1) ≈ 0.367...
    posit16 p_neg_one      = posit16(-1.0);
    posit16 result_neg_one = p_neg_one.exp();
    posit16 expected_inv_e = posit16(std::exp(-1.0));
    ASSERT_EQ(result_neg_one.value, expected_inv_e.value)
        << "exp(-1) = " << result_neg_one.toDouble() << " but expected "
        << expected_inv_e.toDouble();

    // NaR input should result in NaR
    posit16 p_nar;
    p_nar.toNaR();
    posit16 result_nar = p_nar.exp();
    ASSERT_TRUE(result_nar.isNaR())
        << "exp(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test the identity: e^(a+b) = e^a * e^b
TEST(Posit16MathFunctions, ExpAdditiveProperty)
{
    // Create a distribution that generates values in a safe range to avoid
    // overflow
    std::uniform_real_distribution<double> safe_dist(-5.0, 5.0);

    for (int i = 0; i < NTESTS16 / 10; i++)
    {
        double f_a = safe_dist(gen);
        double f_b = safe_dist(gen);

        posit16 p_a = posit16(f_a);
        posit16 p_b = posit16(f_b);

        // Left side: e^(a+b)
        posit16 sum     = p_a + p_b;
        posit16 exp_sum = sum.exp();

        // Right side: e^a * e^b
        posit16 exp_a   = p_a.exp();
        posit16 exp_b   = p_b.exp();
        posit16 product = exp_a * exp_b;

        // Allow slightly higher tolerance for this complex operation
        ASSERT_TRUE(double_eq(exp_sum.toDouble(), product.toDouble(), 1e-1))
            << "Failed: exp(" << p_a.toDouble() << " + " << p_b.toDouble()
            << ") = " << exp_sum.toDouble() << " but exp(" << p_a.toDouble() << ") * exp("
            << p_b.toDouble() << ") = " << product.toDouble();
    }
}

// Tests for p16_exp2 implementation
TEST(Posit16MathFunctions, Exp2Function)
{
    std::uniform_int_distribution<uint16_t> full_dist(0, 0xFFFF);

    for (int i = 0; i < NTESTS16; i++)
    {
        posit16 p_a;
        p_a.value = full_dist(gen);

        if (p_a.isNaR())
        {
            continue;
        }

        double  f_a        = p_a.toDouble();
        posit16 p_result   = p_a.exp2();
        double  f_result   = std::pow(2.0, f_a);
        posit16 p_expected = posit16(f_result);

        if (p_expected.value == 0 || p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed exp2: exp2(" << f_a << ") = " << p_result.toDouble() << " but expected "
            << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value << " -> 0x"
            << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}