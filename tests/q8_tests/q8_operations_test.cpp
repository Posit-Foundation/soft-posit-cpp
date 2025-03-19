// tests/q8_tests/q8_operations_test.cpp
#include <gtest/gtest.h>
#include <cmath>
#include "softposit_cpp.h"
#include "../common_tests/test_helpers.h"
#include "../common_tests/test_constants.h"
#include "../common_tests/random_generators.h"

using namespace softposit_test;

//========================================================================================
// QUIRE8 OPERATIONS TESTS
//========================================================================================

// Test for quire_mul_add
TEST(Quire8Operations, MulAdd)
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

        // Create and initialize quire
        quire8 q;
        q.clr();

        // Perform operations (q += (p_a * p_b); q += p_c;)
        q.qma(p_a, p_b);
        q.qma(posit8(1.0), p_c);

        // Convert back to posit
        posit8 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(f_a, f_b, f_c);
        posit8 expected = f;

        // Allow 1 ULP difference
        ASSERT_LE(ulp_p8(p, expected), 1)
            << "Input: (" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

// Test for quire_mul_sub
TEST(Quire8Operations, MulSub)
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

        // Create and initialize quire
        quire8 q;
        q.clr();

        // Perform operations (q -= (p_a * p_b); q += p_c;)
        q.qms(p_a, p_b);
        q.qma(posit8(1.0), p_c);

        // Convert back to posit
        posit8 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(-f_a, f_b, f_c);
        posit8 expected = f;

        // Allow 1 ULP difference
        ASSERT_LE(ulp_p8(p, expected), 1)
            << "Input: (-" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

// Test multiple operations on the same quire
TEST(Quire8Operations, MultipleOperations)
{
    for (int i = 0; i < NTESTS8 / 10; i++) // Reduced test count for multiple operations
    {
        // Create and initialize quire
        quire8 q;
        q.clr();

        // Track the expected result using double precision
        double expected_result = 0.0;

        // Perform multiple random operations
        for (int j = 0; j < 5; j++)
        {
            posit8 p_a = RandomGenerators::randomPosit8();
            posit8 p_b = RandomGenerators::randomPosit8();

            double f_a = p_a.toDouble();
            double f_b = p_b.toDouble();

            // Decide randomly between add and subtract
            bool add_operation = (RandomGenerators::gen() % 2 == 0);

            if (add_operation)
            {
                q.qma(p_a, p_b);
                expected_result += f_a * f_b;
            }
            else
            {
                q.qms(p_a, p_b);
                expected_result -= f_a * f_b;
            }
        }

        // Convert quire to posit
        posit8 result = q.toPosit();
        posit8 expected = expected_result;

        // Allow slightly larger ULP difference due to accumulated operations
        ASSERT_LE(ulp_p8(result, expected), 2)
            << "Multiple operations result: " << result.toDouble()
            << "\nExpected: " << expected_result;
    }
}

// Test edge cases
TEST(Quire8Operations, EdgeCases)
{
    // Test with zero values
    quire8 q_zero;
    q_zero.clr();
    q_zero.qma(posit8(0.0), posit8(0.0));
    EXPECT_EQ(q_zero.toPosit().value, posit8(0.0).value);

    // Test with NAR
    quire8 q_nar;
    q_nar.clr();
    posit8 nar;
    nar.toNaR();
    q_nar.qma(nar, posit8(1.0));
    EXPECT_TRUE(q_nar.toPosit().isNaR());

    // Test with extreme values
    quire8 q_extreme;
    q_extreme.clr();
    // Add maximum values several times to try to cause overflow
    for (int i = 0; i < 10; i++)
    {
        q_extreme.qma(posit8(64.0), posit8(64.0)); // Max posit8 value is around 64
    }
    // Should not be NAR (quire has higher capacity than posit)
    EXPECT_FALSE(q_extreme.toPosit().isNaR());
}