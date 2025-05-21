#include "common_tests/test_utils.h"
#include <cmath>
#include <random>

// Test for p8_sin_pi function implementation
TEST(Posit8MathFunctions, SinPiFunction) {
  current_operation = "Sine Function (sin(π*x))";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    // Calculate sin(π*x) using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.sin_pi();
    double f_result = std::sin(M_PI * f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed sin_pi: sin(π*" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for sin_pi function
TEST(Posit8MathFunctions, SinPiSpecificValues) {
  current_operation = "Sine Function (sin(π*x)) Special Cases";

  // sin(π*0) = sin(0) = 0
  posit8 zero = posit8(0.0);
  posit8 result_zero = zero.sin_pi();
  ASSERT_EQ(result_zero.value, zero.value)
      << "sin(π*0) = " << result_zero.toDouble() << " but expected 0.0";

  // sin(π*0.5) = sin(π/2) = 1
  posit8 half = posit8(0.5);
  posit8 one = posit8(1.0);
  posit8 result_half = half.sin_pi();
  ASSERT_EQ(result_half.value, one.value)
      << "sin(π*0.5) = " << result_half.toDouble() << " but expected 1.0";

  // sin(π*1) = sin(π) = 0
  posit8 p_one = posit8(1.0);
  posit8 result_one = p_one.sin_pi();
  posit8 expected_zero = posit8(0.0);
  ASSERT_EQ(result_one.value, expected_zero.value)
      << "sin(π*1) = " << result_one.toDouble() << " but expected 0.0";

  // sin(π*(-0.5)) = sin(-π/2) = -1
  posit8 neg_half = posit8(-0.5);
  posit8 neg_one = posit8(-1.0);
  posit8 result_neg_half = neg_half.sin_pi();
  ASSERT_EQ(result_neg_half.value, neg_one.value)
      << "sin(π*(-0.5)) = " << result_neg_half.toDouble() << " but expected -1.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.sin_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "sin_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p8_cos_pi function implementation
TEST(Posit8MathFunctions, CosPiFunction) {
  current_operation = "Cosine Function (cos(π*x))";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    // Calculate cos(π*x) using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.cos_pi();
    double f_result = std::cos(M_PI * f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed cos_pi: cos(π*" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for cos_pi function
TEST(Posit8MathFunctions, CosPiSpecificValues) {
  current_operation = "Cosine Function (cos(π*x)) Special Cases";

  // cos(π*0) = cos(0) = 1
  posit8 zero = posit8(0.0);
  posit8 one = posit8(1.0);
  posit8 result_zero = zero.cos_pi();
  ASSERT_EQ(result_zero.value, one.value)
      << "cos(π*0) = " << result_zero.toDouble() << " but expected 1.0";

  // cos(π*0.5) = cos(π/2) = 0
  posit8 half = posit8(0.5);
  posit8 result_half = half.cos_pi();
  posit8 expected_zero = posit8(0.0);
  ASSERT_EQ(result_half.value, expected_zero.value)
      << "cos(π*0.5) = " << result_half.toDouble() << " but expected 0.0";

  // cos(π*1) = cos(π) = -1
  posit8 p_one = posit8(1.0);
  posit8 neg_one = posit8(-1.0);
  posit8 result_one = p_one.cos_pi();
  ASSERT_EQ(result_one.value, neg_one.value)
      << "cos(π*1) = " << result_one.toDouble() << " but expected -1.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.cos_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "cos_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p8_tan_pi function implementation
TEST(Posit8MathFunctions, TanPiFunction) {
  current_operation = "Tangent Function (tan(π*x))";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    double f_a = p_a.toDouble();

    // Skip values where tan(π*x) is undefined (x = n + 0.5 where n is an integer)
    double mod_value = std::fabs(f_a - std::round(f_a - 0.5));
    if (mod_value < 0.01) {
      continue;
    }

    // Calculate tan(π*x) using both methods
    posit8 p_result = p_a.tan_pi();
    double f_result = std::tan(M_PI * f_a);
    posit8 p_expected = posit8(f_result);

    // Skip comparison if the result is outside the posit8 range
    if (std::abs(f_result) > 100.0) {
      continue;
    }

    // Allow slightly higher tolerance for tangent due to higher sensitivity
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 2e-1))
        << "Failed tan_pi: tan(π*" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for tan_pi function
TEST(Posit8MathFunctions, TanPiSpecificValues) {
  current_operation = "Tangent Function (tan(π*x)) Special Cases";

  // tan(π*0) = tan(0) = 0
  posit8 zero = posit8(0.0);
  posit8 result_zero = zero.tan_pi();
  ASSERT_EQ(result_zero.value, zero.value)
      << "tan(π*0) = " << result_zero.toDouble() << " but expected 0.0";

  // tan(π*1) = tan(π) = 0
  posit8 p_one = posit8(1.0);
  posit8 result_one = p_one.tan_pi();
  posit8 expected_zero = posit8(0.0);
  ASSERT_EQ(result_one.value, expected_zero.value)
      << "tan(π*1) = " << result_one.toDouble() << " but expected 0.0";

  // tan(π*0.25) = tan(π/4) = 1
  posit8 quarter = posit8(0.25);
  posit8 one = posit8(1.0);
  posit8 result_quarter = quarter.tan_pi();
  ASSERT_EQ(result_quarter.value, one.value)
      << "tan(π*0.25) = " << result_quarter.toDouble() << " but expected 1.0";

  // tan(π*(-0.25)) = tan(-π/4) = -1
  posit8 neg_quarter = posit8(-0.25);
  posit8 neg_one = posit8(-1.0);
  posit8 result_neg_quarter = neg_quarter.tan_pi();
  ASSERT_EQ(result_neg_quarter.value, neg_one.value)
      << "tan(π*(-0.25)) = " << result_neg_quarter.toDouble() << " but expected -1.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.tan_pi();
  ASSERT_TRUE(result_nar.isNaR())
      << "tan_pi(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test Pythagorean identity: sin²(π*x) + cos²(π*x) = 1
TEST(Posit8MathFunctions, PythagoreanIdentity) {
  current_operation = "Pythagorean Identity";

  // Create a distribution that generates values across a reasonable range
  std::uniform_real_distribution<double> dist(-0.9, 0.9);

  for (int i = 0; i < NTESTS8 / 10; i++) {
    double f_a = dist(gen);
    posit8 p_a = posit8(f_a);

    // Calculate sin(π*x) and cos(π*x)
    posit8 sin_result = p_a.sin_pi();
    posit8 cos_result = p_a.cos_pi();

    // Calculate sin²(π*x) + cos²(π*x)
    posit8 sin_sq = sin_result * sin_result;
    posit8 cos_sq = cos_result * cos_result;
    posit8 sum = sin_sq + cos_sq;

    // The result should be approximately 1
    posit8 one = posit8(1.0);

    // Allow slightly higher tolerance for this identity check
    ASSERT_TRUE(double_eq(sum.toDouble(), one.toDouble(), 1e-12, 2e-1))
        << "Failed Pythagorean identity: sin²(π*" << p_a.toDouble() << ") + cos²(π*" 
        << p_a.toDouble() << ") = " << sum.toDouble() << " but expected 1.0";
  }
} 