#include <gtest/gtest.h>

#include <random>

#include "common_tests/test_utils.h"

// POSIT8 ARITHMETIC TESTS

// Test case for addition
TEST(Posit8Arithmetic, Add)
{
    test_exact<posit8>(
        NTESTS8,
        // Posit operation
        [](posit8 a, posit8 b) { return a + b; },
        // Equivalent double operation
        [](double a, double b) { return a + b; },
        // Random generator
        []() { return int_dist8(gen); });
}

// Test case for subtraction
TEST(Posit8Arithmetic, Sub)
{
    test_exact<posit8>(
        NTESTS8,
        // Posit operation
        [](posit8 a, posit8 b) { return a - b; },
        // Equivalent double operation
        [](double a, double b) { return a - b; },
        // Random generator
        []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test case for multiplication
TEST(Posit8Arithmetic, Mul)
{
    test_exact<posit8>(
        NTESTS8,
        // Posit operation
        [](posit8 a, posit8 b) { return a * b; },
        // Equivalent double operation
        [](double a, double b) { return a * b; },
        // Random generator
        []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test case for division
TEST(Posit8Arithmetic, Div)
{
    test_exact<posit8>(
        NTESTS8,
        // Posit operation
        [](posit8 a, posit8 b)
        {
            // Skip division by zero
            if (b.value == 0 || b.isNaR())
            {
                return posit8(0);
            }
            return a / b;
        },
        // Equivalent double operation
        [](double a, double b)
        {
            if (b == 0 || std::isnan(b) || std::isinf(b))
            {
                return 0.0;
            }
            return a / b;
        },
        // Random generator
        []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test square root operation
TEST(Posit8Arithmetic, Sqrt)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit value
        posit8 p_a;
        p_a.value = int_dist8(gen);

        double f_a = p_a.toDouble();

        // Skip negative values for sqrt and NaN values
        if (f_a < 0 || std::isnan(f_a) || p_a.isNaR())
            continue;

        posit8 p_result = p_a;
        p_result.sqrt();

        double f_result = std::sqrt(f_a);
        posit8 expected = f_result;

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "sqrt(" << f_a << ") = " << f_result << " but got " << p_result.toDouble();
    }
}

// Test rounding operation
TEST(Posit8Arithmetic, Round)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit value
        posit8 p_a;
        p_a.value = int_dist8(gen);

        double f_a = p_a.toDouble();

        // Skip NaN values
        if (std::isnan(f_a) || p_a.isNaR())
            continue;

        posit8 p_result = p_a;
        p_result.rint();

        double f_result = std::round(f_a);
        posit8 expected = f_result;

        // Skip values at exactly 0.5 boundary like in Rust test
        if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10)
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "round(" << f_a << ") = " << f_result << " but got " << p_result.toDouble();
    }
}

// Test fused multiply-add operation
TEST(Posit8Arithmetic, MulAdd)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit values
        posit8 p_a, p_b, p_c;
        p_a.value = int_dist8(gen);
        p_b.value = int_dist8(gen);
        p_c.value = int_dist8(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Skip NaN/NaR values
        if (p_a.isNaR() || p_b.isNaR() || p_c.isNaR() || std::isnan(f_a) || std::isnan(f_b) ||
            std::isnan(f_c))
        {
            continue;
        }

        // Test global function
        posit8 p_result1 = fma(p_a, p_b, p_c);
        // Test member function
        posit8 p_result2 = p_c.fma(p_a, p_b);

        double f_result = std::fma(f_a, f_b, f_c);
        posit8 expected = f_result;

        // Allow up to 1 ULP difference for posit8
        auto ulp_diff1 = ulp(p_result1, expected);
        auto ulp_diff2 = ulp(p_result2, expected);

        ASSERT_LE(ulp_diff1, 1) << "fma(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
                                << " but got " << p_result1.toDouble()
                                << " (ULP diff: " << ulp_diff1 << ")";
        ASSERT_LE(ulp_diff2, 1) << "p_c.fma(" << f_a << ", " << f_b << ") = " << f_result
                                << " but got " << p_result2.toDouble()
                                << " (ULP diff: " << ulp_diff2 << ")";
    }
}

// Test fused multiply-subtract operation (a*b-c)
TEST(Posit8Arithmetic, MulSub)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit values
        posit8 p_a, p_b, p_c;
        p_a.value = int_dist8(gen);
        p_b.value = int_dist8(gen);
        p_c.value = int_dist8(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Skip NaN/NaR values
        if (p_a.isNaR() || p_b.isNaR() || p_c.isNaR() || std::isnan(f_a) || std::isnan(f_b) ||
            std::isnan(f_c))
        {
            continue;
        }

        // Test global function
        posit8 p_result1 = fms(p_a, p_b, p_c);
        // Test member function
        posit8 p_result2 = p_c.fms(p_a, p_b);

        double f_result = f_a * f_b - f_c;
        posit8 expected = f_result;

        // Allow up to 1 ULP difference for posit8
        auto ulp_diff1 = ulp(p_result1, expected);
        auto ulp_diff2 = ulp(p_result2, expected);

        ASSERT_LE(ulp_diff1, 1) << "fms(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
                                << " but got " << p_result1.toDouble()
                                << " (ULP diff: " << ulp_diff1 << ")";
        ASSERT_LE(ulp_diff2, 1) << "p_c.fms(" << f_a << ", " << f_b << ") = " << f_result
                                << " but got " << p_result2.toDouble()
                                << " (ULP diff: " << ulp_diff2 << ")";
    }
}

// Test negative fused multiply-add operation (c-a*b)
TEST(Posit8Arithmetic, SubMul)
{
    for (int i = 0; i < NTESTS8; i++)
    {
        // Generate random posit values
        posit8 p_a, p_b, p_c;
        p_a.value = int_dist8(gen);
        p_b.value = int_dist8(gen);
        p_c.value = int_dist8(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Skip NaN/NaR values
        if (p_a.isNaR() || p_b.isNaR() || p_c.isNaR() || std::isnan(f_a) || std::isnan(f_b) ||
            std::isnan(f_c))
        {
            continue;
        }

        // Test global function
        posit8 p_result1 = nfma(p_a, p_b, p_c);
        // Test member function
        posit8 p_result2 = p_c.nfma(p_a, p_b);

        double f_result = f_c - (f_a * f_b);
        posit8 expected = f_result;

        // Allow up to 1 ULP difference for posit8
        auto ulp_diff1 = ulp(p_result1, expected);
        auto ulp_diff2 = ulp(p_result2, expected);

        ASSERT_LE(ulp_diff1, 1) << "nfma(" << f_a << ", " << f_b << ", " << f_c
                                << ") = " << f_result << " but got " << p_result1.toDouble()
                                << " (ULP diff: " << ulp_diff1 << ")";
        ASSERT_LE(ulp_diff2, 1) << "p_c.nfma(" << f_a << ", " << f_b << ") = " << f_result
                                << " but got " << p_result2.toDouble()
                                << " (ULP diff: " << ulp_diff2 << ")";
    }
}