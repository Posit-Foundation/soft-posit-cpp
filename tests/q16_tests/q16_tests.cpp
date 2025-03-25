#include "common_tests/test_utils.h"
#include <gtest/gtest.h>
#include <random>

// Test for quire_mul_add
TEST(Quire16Operations, MulAdd) {
  current_operation = "Quire Multiply-Add";
  for (int i = 0; i < NTESTS16; i++) {
    // Generate random posit values
    posit16 p_a, p_b, p_c;
    p_a.value = int_dist16(gen);
    p_b.value = int_dist16(gen);
    p_c.value = int_dist16(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire16 q;
    q.clr();

    // Perform operations (q += (p_a, p_b); q += p_c;)
    q.qma(p_a, p_b);
    q.qma(posit16(1.0), p_c);

    // Convert back to posit
    posit16 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(f_a, f_b, f_c);
    posit16 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

// Test for quire_mul_sub
TEST(Quire16Operations, MulSub) {
  current_operation = "Quire Multiply-Subtract";
  for (int i = 0; i < NTESTS16; i++) {
    // Generate random posit values
    posit16 p_a, p_b, p_c;
    p_a.value = int_dist16(gen);
    p_b.value = int_dist16(gen);
    p_c.value = int_dist16(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire16 q;
    q.clr();

    // Perform operations (q -= (p_a, p_b); q += p_c;)
    q.qms(p_a, p_b);
    q.qma(posit16(1.0), p_c);

    // Convert back to posit
    posit16 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(-f_a, f_b, f_c);
    posit16 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (-" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}