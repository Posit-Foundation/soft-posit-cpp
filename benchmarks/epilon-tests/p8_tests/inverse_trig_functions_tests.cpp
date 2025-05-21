#include "common_tests/test_utils.h"
#include <cmath>
#include <random>

// Test for p8_asin_pi function implementation
TEST(Posit8MathFunctions, AsinPiFunction) {
  current_operation = "Inverse Sine Function (asin(x)/π)";

  // Create a distribution that generates values in the valid range for asin [-1, 1]
  std::uniform_real_distribution<double> valid_dist(-0.99, 0.99);

  for (int i = 0; i < NTESTS8; i++) {
    double f_a = valid_dist(gen);
    posit8 p_a = posit8(f_a);

    // Calculate asin(x)/π using both methods
    posit8 p_result = p_a.asin_pi();
    double f_result = std::asin(f_a) / M_PI;
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed asin_pi: asin(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for asin_pi function
TEST(Posit8MathFunctions, AsinPiSpecificValues) {
  current_operation = "Inverse Sine Function (asin(x)/π) Special Cases";

  // asin(0)/π = 0
  posit8 zero = posit8(0.0);
  posit8 result_zero = zero.asin_pi();
  ASSERT_EQ(result_zero.value, zero.value)
      << "asin(0)/π = " << result_zero.toDouble() << " but expected 0.0";

  // asin(1)/π = 0.5
  posit8 one = posit8(1.0);
  posit8 half = posit8(0.5);
  posit8 result_one = one.asin_pi();
  ASSERT_EQ(result_one.value, half.value)
      << "asin(1)/π = " << result_one.toDouble() << " but expected 0.5";

  // asin(-1)/π = -0.5
  posit8 neg_one = posit8(-1.0);
  posit8 neg_half = posit8(-0.5);
  posit8 result_neg_one = neg_one.asin_pi();
  ASSERT_EQ(result_neg_one.value, neg_half.value)
      << "asin(-1)/π = " << result_neg_one.toDouble() << " but expected -0.5";

  // Input outside [-1, 1] should result in NaR
  posit8 outside_range = posit8(2.0);
  posit8 result_outside = outside_range.asin_pi();
  ASSERT_TRUE(result_outside.isNaR())
      << "asin(2)/π = " << result_outside.toDouble() << " but expected NaR";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.asin_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "asin_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p8_acos_pi function implementation
TEST(Posit8MathFunctions, AcosPiFunction) {
  current_operation = "Inverse Cosine Function (acos(x)/π)";

  // Create a distribution that generates values in the valid range for acos [-1, 1]
  std::uniform_real_distribution<double> valid_dist(-0.99, 0.99);

  for (int i = 0; i < NTESTS8; i++) {
    double f_a = valid_dist(gen);
    posit8 p_a = posit8(f_a);

    // Calculate acos(x)/π using both methods
    posit8 p_result = p_a.acos_pi();
    double f_result = std::acos(f_a) / M_PI;
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed acos_pi: acos(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for acos_pi function
TEST(Posit8MathFunctions, AcosPiSpecificValues) {
  current_operation = "Inverse Cosine Function (acos(x)/π) Special Cases";

  // acos(1)/π = 0
  posit8 one = posit8(1.0);
  posit8 zero = posit8(0.0);
  posit8 result_one = one.acos_pi();
  ASSERT_EQ(result_one.value, zero.value)
      << "acos(1)/π = " << result_one.toDouble() << " but expected 0.0";

  // acos(0)/π = 0.5
  posit8 p_zero = posit8(0.0);
  posit8 half = posit8(0.5);
  posit8 result_zero = p_zero.acos_pi();
  ASSERT_EQ(result_zero.value, half.value)
      << "acos(0)/π = " << result_zero.toDouble() << " but expected 0.5";

  // acos(-1)/π = 1
  posit8 neg_one = posit8(-1.0);
  posit8 p_one = posit8(1.0);
  posit8 result_neg_one = neg_one.acos_pi();
  ASSERT_EQ(result_neg_one.value, p_one.value)
      << "acos(-1)/π = " << result_neg_one.toDouble() << " but expected 1.0";

  // Input outside [-1, 1] should result in NaR
  posit8 outside_range = posit8(2.0);
  posit8 result_outside = outside_range.acos_pi();
  ASSERT_TRUE(result_outside.isNaR())
      << "acos(2)/π = " << result_outside.toDouble() << " but expected NaR";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.acos_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "acos_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p8_atan_pi function implementation
TEST(Posit8MathFunctions, AtanPiFunction) {
  current_operation = "Inverse Tangent Function (atan(x)/π)";

  // Create a distribution that generates values across a reasonable range
  // atan is defined for all real values, but we'll use a limited range to avoid extreme values
  std::uniform_real_distribution<double> safe_dist(-10.0, 10.0);

  for (int i = 0; i < NTESTS8; i++) {
    double f_a = safe_dist(gen);
    posit8 p_a = posit8(f_a);

    // Calculate atan(x)/π using both methods
    posit8 p_result = p_a.atan_pi();
    double f_result = std::atan(f_a) / M_PI;
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed atan_pi: atan(" << p_a.toDouble() << ")/π = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for atan_pi function
TEST(Posit8MathFunctions, AtanPiSpecificValues) {
  current_operation = "Inverse Tangent Function (atan(x)/π) Special Cases";

  // atan(0)/π = 0
  posit8 zero = posit8(0.0);
  posit8 result_zero = zero.atan_pi();
  ASSERT_EQ(result_zero.value, zero.value)
      << "atan(0)/π = " << result_zero.toDouble() << " but expected 0.0";

  // atan(1)/π = 0.25
  posit8 one = posit8(1.0);
  posit8 quarter = posit8(0.25);
  posit8 result_one = one.atan_pi();
  ASSERT_EQ(result_one.value, quarter.value)
      << "atan(1)/π = " << result_one.toDouble() << " but expected 0.25";

  // atan(-1)/π = -0.25
  posit8 neg_one = posit8(-1.0);
  posit8 neg_quarter = posit8(-0.25);
  posit8 result_neg_one = neg_one.atan_pi();
  ASSERT_EQ(result_neg_one.value, neg_quarter.value)
      << "atan(-1)/π = " << result_neg_one.toDouble() << " but expected -0.25";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.atan_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "atan_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test inverse relationships between sin_pi and asin_pi
TEST(Posit8MathFunctions, SinAsinInverse) {
  current_operation = "Sine and Inverse Sine Inverse Relationship";

  // Create a distribution for values in the range [-0.5, 0.5]
  // This corresponds to the range [-π/2, π/2] for x in sin(π*x)
  std::uniform_real_distribution<double> dist(-0.5, 0.5);

  for (int i = 0; i < NTESTS8 / 10; i++) {
    double f_a = dist(gen);
    posit8 p_a = posit8(f_a);

    // Calculate sin(π*x)
    posit8 sin_result = p_a.sin_pi();
    
    // Calculate asin(sin(π*x))/π - should get back x
    posit8 asin_result = sin_result.asin_pi();

    // Allow slightly higher tolerance for this composite operation
    ASSERT_TRUE(double_eq(asin_result.toDouble(), p_a.toDouble(), 1e-12, 2e-1))
        << "Failed inverse relationship: asin(sin(π*" << p_a.toDouble() << "))/π = " 
        << asin_result.toDouble() << " but expected " << p_a.toDouble();
  }
} 