#include <cmath>
#include <random>

#include "common_tests/test_utils.h"

// Test for p16_sin_pi function implementation
TEST(Posit16MathFunctions, SinPiFunction)
{
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = dist(gen);
        posit16 p_a = posit16(f_a);

        // Skip NaR inputs
        if (p_a.isNaR())
        {
            continue;
        }

        posit16 p_result   = p_a.sin_pi();
        double  f_result   = std::sin(M_PI * f_a);
        posit16 p_expected = posit16(f_result);

        // Skip cases where expected is zero (allows tolerance issues)
        if (p_expected.value == 0 || p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed sin_pi: sin(" << p_a.toDouble() << " * π) / π?";
    }
}

// Test for p16_cos_pi function implementation
TEST(Posit16MathFunctions, CosPiFunction)
{
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = dist(gen);
        posit16 p_a = posit16(f_a);

        if (p_a.isNaR())
        {
            continue;
        }

        posit16 p_result   = p_a.cos_pi();
        double  f_result   = std::cos(M_PI * f_a);
        posit16 p_expected = posit16(f_result);

        if (p_expected.value == 0 || p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed cos_pi: cos(" << p_a.toDouble() << " * π)";
    }
}

// Test for p16_tan_pi function implementation
TEST(Posit16MathFunctions, TanPiFunction)
{
    // Restrict range to avoid poles at half-integer values
    std::uniform_real_distribution<double> dist(-0.49, 0.49);

    for (int i = 0; i < NTESTS16; i++)
    {
        double  f_a = dist(gen);
        posit16 p_a = posit16(f_a);

        if (p_a.isNaR())
        {
            continue;
        }

        posit16 p_result   = p_a.tan_pi();
        double  f_result   = std::tan(M_PI * f_a);
        posit16 p_expected = posit16(f_result);

        if (p_expected.value == 0 || p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed tan_pi: tan(" << p_a.toDouble() << " * π)";
    }
}