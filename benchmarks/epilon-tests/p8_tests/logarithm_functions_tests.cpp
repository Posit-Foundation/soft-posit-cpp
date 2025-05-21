#include "common_tests/test_utils.h"
#include <cmath>
#include <random>

// Test for p8_ln function implementation
TEST(Posit8MathFunctions, LnFunction) {
  current_operation = "Natural Logarithm Function";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values and non-positive values for input
    if (p_a.isNaR() || p_a.toDouble() <= 0.0) {
      continue;
    }

    // Calculate natural logarithm using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.ln();
    double f_result = std::log(f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed ln: ln(" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for natural logarithm function
TEST(Posit8MathFunctions, LnSpecificValues) {
  current_operation = "Natural Logarithm Function Special Cases";

  // ln(1) = 0
  posit8 one = posit8(1.0);
  posit8 zero = posit8(0.0);
  posit8 result_one = one.ln();
  ASSERT_EQ(result_one.value, zero.value)
      << "ln(1) = " << result_one.toDouble() << " but expected 0.0";

  // ln(e) ≈ 1
  posit8 p_e = posit8(std::exp(1.0));
  posit8 result_e = p_e.ln();
  posit8 expected_one = posit8(1.0);
  ASSERT_EQ(result_e.value, expected_one.value)
      << "ln(e) = " << result_e.toDouble() << " but expected 1.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.ln();
  ASSERT_TRUE(result_nar.isNaR())
      << "ln(NaR) = " << result_nar.toDouble() << " but expected NaR";

  // Non-positive input should result in NaR
  posit8 p_zero = posit8(0.0);
  posit8 result_zero = p_zero.ln();
  ASSERT_TRUE(result_zero.isNaR())
      << "ln(0) = " << result_zero.toDouble() << " but expected NaR";

  posit8 p_neg = posit8(-1.0);
  posit8 result_neg = p_neg.ln();
  ASSERT_TRUE(result_neg.isNaR())
      << "ln(-1) = " << result_neg.toDouble() << " but expected NaR";
}

// Test for p8_log2 function implementation
TEST(Posit8MathFunctions, Log2Function) {
  current_operation = "Base-2 Logarithm Function";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values and non-positive values for input
    if (p_a.isNaR() || p_a.toDouble() <= 0.0) {
      continue;
    }

    // Calculate base-2 logarithm using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.log2();
    double f_result = std::log2(f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed log2: log2(" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for base-2 logarithm function
TEST(Posit8MathFunctions, Log2SpecificValues) {
  current_operation = "Base-2 Logarithm Function Special Cases";

  // log2(1) = 0
  posit8 one = posit8(1.0);
  posit8 zero = posit8(0.0);
  posit8 result_one = one.log2();
  ASSERT_EQ(result_one.value, zero.value)
      << "log2(1) = " << result_one.toDouble() << " but expected 0.0";

  // log2(2) = 1
  posit8 p_two = posit8(2.0);
  posit8 result_two = p_two.log2();
  posit8 expected_one = posit8(1.0);
  ASSERT_EQ(result_two.value, expected_one.value)
      << "log2(2) = " << result_two.toDouble() << " but expected 1.0";

  // log2(4) = 2
  posit8 p_four = posit8(4.0);
  posit8 result_four = p_four.log2();
  posit8 expected_two = posit8(2.0);
  ASSERT_EQ(result_four.value, expected_two.value)
      << "log2(4) = " << result_four.toDouble() << " but expected 2.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.log2();
  ASSERT_TRUE(result_nar.isNaR())
      << "log2(NaR) = " << result_nar.toDouble() << " but expected NaR";

  // Non-positive input should result in NaR
  posit8 p_zero = posit8(0.0);
  posit8 result_zero = p_zero.log2();
  ASSERT_TRUE(result_zero.isNaR())
      << "log2(0) = " << result_zero.toDouble() << " but expected NaR";

  posit8 p_neg = posit8(-1.0);
  posit8 result_neg = p_neg.log2();
  ASSERT_TRUE(result_neg.isNaR())
      << "log2(-1) = " << result_neg.toDouble() << " but expected NaR";
}

// Test the identity: ln(a*b) = ln(a) + ln(b)
TEST(Posit8MathFunctions, LnMultiplicativeProperty) {
  current_operation = "Natural Logarithm Multiplicative Property";

  // Create a distribution that generates positive values in a safe range
  std::uniform_real_distribution<double> safe_dist(0.5, 2.0);

  for (int i = 0; i < NTESTS8 / 10; i++) {
    double f_a = safe_dist(gen);
    double f_b = safe_dist(gen);

    posit8 p_a = posit8(f_a);
    posit8 p_b = posit8(f_b);

    // Left side: ln(a*b)
    posit8 product = p_a * p_b;
    posit8 ln_product = product.ln();

    // Right side: ln(a) + ln(b)
    posit8 ln_a = p_a.ln();
    posit8 ln_b = p_b.ln();
    posit8 sum = ln_a + ln_b;

    // Allow slightly higher tolerance for this complex operation
    ASSERT_TRUE(double_eq(ln_product.toDouble(), sum.toDouble(), 1e-12, 1e-1))
        << "Failed: ln(" << p_a.toDouble() << " * " << p_b.toDouble()
        << ") = " << ln_product.toDouble() << " but ln(" << p_a.toDouble()
        << ") + ln(" << p_b.toDouble() << ") = " << sum.toDouble();
  }
} 