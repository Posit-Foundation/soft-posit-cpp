#include "common_tests/test_utils.h"
#include <cmath>
#include <random>

// Test for p8_exp function implementation
TEST(Posit8MathFunctions, ExpFunction) {
  current_operation = "Exponential Function";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    // Calculate exponential using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.exp();
    double f_result = std::exp(f_a);
    posit8 p_expected = posit8(f_result);

    // Skip comparison if expected result is zero or NaR
    if (p_expected.value == 0 || p_expected.isNaR()) {
      continue;
    }

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed exp: exp(" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for exponential function
TEST(Posit8MathFunctions, ExpSpecificValues) {
  current_operation = "Exponential Function Special Cases";

  // exp(0) = 1
  posit8 zero = posit8(0.0);
  posit8 one = posit8(1.0);
  posit8 result_zero = zero.exp();
  ASSERT_EQ(result_zero.value, one.value)
      << "exp(0) = " << result_zero.toDouble() << " but expected 1.0";

  // exp(1) ≈ 2.718...
  posit8 p_one = posit8(1.0);
  posit8 result_one = p_one.exp();
  posit8 expected_e = posit8(std::exp(1.0));
  ASSERT_EQ(result_one.value, expected_e.value)
      << "exp(1) = " << result_one.toDouble() << " but expected "
      << expected_e.toDouble();

  // exp(-1) ≈ 0.367...
  posit8 p_neg_one = posit8(-1.0);
  posit8 result_neg_one = p_neg_one.exp();
  posit8 expected_inv_e = posit8(std::exp(-1.0));
  ASSERT_EQ(result_neg_one.value, expected_inv_e.value)
      << "exp(-1) = " << result_neg_one.toDouble() << " but expected "
      << expected_inv_e.toDouble();

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.exp();
  ASSERT_TRUE(result_nar.isNaR())
      << "exp(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test the identity: e^(a+b) = e^a * e^b
TEST(Posit8MathFunctions, ExpAdditiveProperty) {
  current_operation = "Exponential Additive Property";

  // Create a distribution that generates values in a safe range to avoid
  // overflow
  std::uniform_real_distribution<double> safe_dist(-1.0, 1.0);

  for (int i = 0; i < NTESTS8 / 10; i++) {
    double f_a = safe_dist(gen);
    double f_b = safe_dist(gen);

    posit8 p_a = posit8(f_a);
    posit8 p_b = posit8(f_b);

    // Left side: e^(a+b)
    posit8 sum = p_a + p_b;
    posit8 exp_sum = sum.exp();

    // Right side: e^a * e^b
    posit8 exp_a = p_a.exp();
    posit8 exp_b = p_b.exp();
    posit8 product = exp_a * exp_b;

    // Allow slightly higher tolerance for this complex operation
    ASSERT_TRUE(double_eq(exp_sum.toDouble(), product.toDouble(), 1e-12, 1e-1))
        << "Failed: exp(" << p_a.toDouble() << " + " << p_b.toDouble()
        << ") = " << exp_sum.toDouble() << " but exp(" << p_a.toDouble()
        << ") * exp(" << p_b.toDouble() << ") = " << product.toDouble();
  }
}

// // Test the identity: (e^a)^b = e^(a*b)
// TEST(Posit8MathFunctions, ExpPowerProperty) {
//   current_operation = "Exponential Power Property";

//   // Create a distribution that generates values in a safe range to avoid
//   // overflow
//   std::uniform_real_distribution<double> safe_dist(-3.0, 3.0);
//   std::uniform_real_distribution<double> small_dist(-2.0, 2.0);

//   for (int i = 0; i < NTESTS8 / 10; i++) {
//     double f_a = safe_dist(gen);
//     double f_b = small_dist(gen); // Use smaller range for exponent

//     posit8 p_a = posit8(f_a);
//     posit8 p_b = posit8(f_b);

//     // Skip cases that might lead to overflow or NaR
//     if (f_a * f_b > 20.0 || f_a * f_b < -20.0) {
//       continue;
//     }

//     // Left side: (e^a)^b
//     posit8 exp_a = p_a.exp();
//     posit8 left_side = exp_a.pow(p_b);

//     // Right side: e^(a*b)
//     posit8 product = p_a * p_b;
//     posit8 right_side = product.exp();

//     // Allow higher tolerance for pow operations
//     ASSERT_TRUE(
//         double_eq(left_side.toDouble(), right_side.toDouble(), 1e-12, 1e-1))
//         << "Failed: (exp(" << p_a.toDouble() << "))^" << p_b.toDouble()
//         << " = " << left_side.toDouble() << " but exp(" << p_a.toDouble()
//         << " * " << p_b.toDouble() << ") = " << right_side.toDouble();
//   }
// } 