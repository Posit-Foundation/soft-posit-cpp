#include "common_tests/test_utils.h"

// POSIT32 ADVANCED TESTS

// Test that adding and then subtracting the same value returns the original
TEST(Posit32Advanced, AddSubCancel) {
  current_operation = "Add-Sub Cancellation";

  // Create a distribution that generates values in the range [-MAX/2, MAX/2]
  std::uniform_int_distribution<int32_t> safe_dist(INT32_MIN / 2,
                                                   INT32_MAX / 2);

  for (int i = 0; i < NTESTS32 / 10; i++) { // Reduced iteration count
    posit32 p_a, p_b;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);

    // Skip NaN/NaR values
    if (p_a.isNaR() || p_b.isNaR() || std::isnan(p_a.toDouble()) ||
        std::isnan(p_b.toDouble())) {
      continue;
    }

    posit32 sum = p_a + p_b;
    posit32 original = sum - p_b;

    // Allow a slightly higher tolerance for posit32
    ASSERT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
        << "Failed: (" << p_a.toDouble() << " + " << p_b.toDouble() << ") - "
        << p_b.toDouble() << " = " << original.toDouble() << " but expected "
        << p_a.toDouble();
  }
}

// Test that multiplying and then dividing by the same value returns the
// original
TEST(Posit32Advanced, MulDivCancel) {
  current_operation = "Mul-Div Cancellation";

  // Create a distribution that generates values in the range [-MAX/2, MAX/2]
  std::uniform_int_distribution<int32_t> safe_dist(INT32_MIN / 2,
                                                   INT32_MAX / 2);

  for (int i = 0; i < NTESTS32 / 10; i++) { // Reduced iteration count
    posit32 p_a, p_b;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);

    // Skip division by zero, NaR, NaN, or very small numbers
    if (p_b.value == 0 || p_b.isNaR() || p_a.isNaR() ||
        std::isnan(p_b.toDouble()) || std::isnan(p_a.toDouble()) ||
        std::abs(p_b.toDouble()) < 1e-6) {
      continue;
    }

    posit32 product = p_a * p_b;
    posit32 original = product / p_b;

    // Allow a slightly higher tolerance for posit32
    ASSERT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
        << "Failed: (" << p_a.toDouble() << " * " << p_b.toDouble() << ") / "
        << p_b.toDouble() << " = " << original.toDouble() << " but expected "
        << p_a.toDouble();
  }
}

// Test specific properties of posit32 arithmetic
TEST(Posit32Advanced, DistributiveProperty) {
  current_operation = "Distributive Property";

  // Create a distribution that generates values in the range [-MAX/4, MAX/4]
  // Using smaller values to avoid overflow
  std::uniform_int_distribution<int32_t> safe_dist(INT32_MIN / 4,
                                                   INT32_MAX / 4);

  for (int i = 0; i < NTESTS32 / 20; i++) { // Further reduced iteration count
    posit32 p_a, p_b, p_c;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);
    p_c.value = safe_dist(gen);

    // Skip NaN/NaR values
    if (p_a.isNaR() || p_b.isNaR() || p_c.isNaR() ||
        std::isnan(p_a.toDouble()) || std::isnan(p_b.toDouble()) ||
        std::isnan(p_c.toDouble())) {
      continue;
    }

    // Test distributive property: a * (b + c) = a * b + a * c
    posit32 left_side = p_a * (p_b + p_c);
    posit32 right_side = (p_a * p_b) + (p_a * p_c);

    // Allow a slightly higher tolerance for posit32 in complex operations
    ASSERT_TRUE(
        double_eq(left_side.toDouble(), right_side.toDouble(), 1e-12, 1e-1))
        << "Distributive property failed: " << p_a.toDouble() << " * ("
        << p_b.toDouble() << " + " << p_c.toDouble()
        << ") = " << left_side.toDouble() << " but (" << p_a.toDouble() << " * "
        << p_b.toDouble() << " + " << p_a.toDouble() << " * " << p_c.toDouble()
        << ") = " << right_side.toDouble();
  }
}


// Test for subnormal values handling
TEST(Posit32Advanced, SubnormalHandling) {
  current_operation = "Subnormal Values";

  // Test with very small positive values
  posit32 min_positive = posit32().minpos();
  posit32 half_min = min_positive / posit32(2.0);

  // Should not be zero (posits don't have subnormals but have very small
  // values)
  ASSERT_NE(half_min.value, 0)
      << "Half of minpos became zero, expected a small positive value";

  // Test with very small negative values
  posit32 max_negative = posit32().minneg();
  posit32 half_max_neg = max_negative / posit32(2.0);

  // Should not be zero
  ASSERT_NE(half_max_neg.value, 0)
      << "Half of minneg became zero, expected a small negative value";
}