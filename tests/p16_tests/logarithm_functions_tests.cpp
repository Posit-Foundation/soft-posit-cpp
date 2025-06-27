#include <cmath>
#include <random>

#include "common_tests/test_utils.h"

// Tests for p16_ln implementation
TEST(Posit16MathFunctions, LnFunction)
{
    std::uniform_int_distribution<uint16_t> full_dist(0, 0x7FFF);  // only positive values

    for (int i = 0; i < NTESTS16; i++)
    {
        posit16 p_a;
        p_a.value = full_dist(gen);

        if (p_a.value == 0 || p_a.isNaR())
        {
            continue;
        }

        double f_a = p_a.toDouble();
        if (f_a <= 0.0)
        {
            continue;
        }  // ln undefined for non-positive

        posit16 p_result   = p_a.ln();
        double  f_result   = std::log(f_a);
        posit16 p_expected = posit16(f_result);

        if (p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed ln: ln(" << f_a << ") = " << p_result.toDouble() << " but expected "
            << p_expected.toDouble();
    }
}

// Tests for p16_log2 implementation
TEST(Posit16MathFunctions, Log2Function)
{
    std::uniform_int_distribution<uint16_t> full_dist(0, 0x7FFF);

    for (int i = 0; i < NTESTS16; i++)
    {
        posit16 p_a;
        p_a.value = full_dist(gen);

        if (p_a.value == 0 || p_a.isNaR())
        {
            continue;
        }

        double f_a = p_a.toDouble();
        if (f_a <= 0.0)
        {
            continue;
        }

        posit16 p_result   = p_a.log2();
        double  f_result   = std::log2(f_a);
        posit16 p_expected = posit16(f_result);

        if (p_expected.isNaR())
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-1))
            << "Failed log2: log2(" << f_a << ") = " << p_result.toDouble() << " but expected "
            << p_expected.toDouble();
    }
}