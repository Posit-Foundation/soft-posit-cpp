// tests/p8_tests/p8_arithmetic_test.cpp
#include <gtest/gtest.h>
#include <cmath>
#include "softposit_cpp.h"
#include "../common_tests/test_helpers.h"
#include "../common_tests/test_constants.h"
#include "../common_tests/random_generators.h"

using namespace softposit_test;

//========================================================================================
// POSIT8 ARITHMETIC TESTS
//========================================================================================

// Test case for addition
TEST(Posit8Arithmetic, Add)
{
    test_exact<posit8>(
        // Posit operation
        [](posit8 a, posit8 b)
        { return a + b; },
        // Equivalent double operation
        [](double a, double b)
        { return a + b; },
        NTESTS8,
        RandomGenerators::randomPosit8);
}

// Test case for subtraction
TEST(Posit8Arithmetic, Sub)
{
    test_exact<posit8>(
        // Posit operation
        [](posit8 a, posit8 b)
        { return a - b; },
        // Equivalent double operation
        [](double a, double b)
        { return a - b; },
        NTESTS8,
        RandomGenerators::randomPosit8);
}

// Test case for multiplication
TEST(Posit8Arithmetic, Mul)
{
    test_exact<posit8>(
        // Posit operation
        [](posit8 a, posit8 b)
        { return a * b; },
        // Equivalent double operation
        [](double a, double b)
        { return a * b; },
        NTESTS8,
        RandomGenerators::randomPosit8);
}

// Test case for division
TEST(Posit8Arithmetic, Div)
{
    test_exact<posit8>(
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
        NTESTS8,
        RandomGenerators::randomPosit8);
}

// Edge cases for arithmetic operations
TEST(Posit8Arithmetic, EdgeCases)
{
    // Test operations with NAR
    posit8 nar;
    nar.toNaR();
    posit8 normal(1.0);

    // Operations with NAR should return NAR
    EXPECT_TRUE((nar + normal).isNaR());
    EXPECT_TRUE((nar - normal).isNaR());
    EXPECT_TRUE((nar * normal).isNaR());
    EXPECT_TRUE((nar / normal).isNaR());
    EXPECT_TRUE((normal / posit8(0.0)).isNaR()); // Division by zero

    // Test operations with zero
    posit8 zero(0.0);
    EXPECT_EQ((zero + zero).value, zero.value);
    EXPECT_EQ((zero - zero).value, zero.value);
    EXPECT_EQ((zero * normal).value, zero.value);
    EXPECT_EQ((normal * zero).value, zero.value);
    EXPECT_EQ((zero / normal).value, zero.value);
}