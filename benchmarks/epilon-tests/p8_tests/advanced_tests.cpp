#include "common_tests/test_utils.h"

// POSIT8 ADVANCED TESTS

// Test that adding and then subtracting the same value returns the original
TEST(Posit8Advanced, AddSubCancel) {
  current_operation = "Add-Sub Cancellation";

  // Create a distribution that generates values in the range [-MAX/2, MAX/2]
  std::uniform_int_distribution<int8_t> safe_dist(INT8_MIN / 2, INT8_MAX / 2);

  for (int i = 0; i < NTESTS8 / 10; i++) { // Reduced iteration count
    posit8 p_a, p_b;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);

    // Skip NaN/NaR values
    if (p_a.isNaR() || p_b.isNaR() || std::isnan(p_a.toDouble()) ||
        std::isnan(p_b.toDouble())) {
      continue;
    }

    posit8 sum = p_a + p_b;
    posit8 original = sum - p_b;

    // Allow a slightly higher tolerance for posit8
    EXPECT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
        << "Failed: (" << p_a.toDouble() << " + " << p_b.toDouble() << ") - "
        << p_b.toDouble() << " = " << original.toDouble() << " but expected "
        << p_a.toDouble();
  }
}

// // Test that multiplying and then dividing by the same value returns the
// // original
// TEST(Posit8Advanced, MulDivCancel) {
//   current_operation = "Mul-Div Cancellation";

//   // Create a distribution that generates values in the range [-MAX/2, MAX/2]
//   std::uniform_int_distribution<int8_t> safe_dist(INT8_MIN / 2, INT8_MAX / 2);

//   for (int i = 0; i < NTESTS8 / 10; i++) { // Reduced iteration count
//     posit8 p_a, p_b;
//     p_a.value = safe_dist(gen);
//     p_b.value = safe_dist(gen);

//     // Skip division by zero, NaR, NaN, or very small numbers
//     if (p_b.value == 0 || p_b.isNaR() || p_a.isNaR() ||
//         std::isnan(p_b.toDouble()) || std::isnan(p_a.toDouble()) ||
//         std::abs(p_b.toDouble()) < 1e-6) {
//       continue;
//     }

//     posit8 product = p_a * p_b;
//     posit8 original = product / p_b;

//     // Allow a slightly higher tolerance for posit8
//     EXPECT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
//         << "Failed: (" << p_a.toDouble() << " * " << p_b.toDouble() << ") / "
//         << p_b.toDouble() << " = " << original.toDouble() << " but expected "
//         << p_a.toDouble();
//   }
// }

// Test for overflow behavior
TEST(Posit8Advanced, OverflowHandling) {
  current_operation = "Overflow Handling";

  // Test overflow with addition
  posit8 max_posit = posit8().maxpos();
  posit8 result = max_posit + max_posit;

  // Should saturate to maxpos, not wrap around or become NaR
  EXPECT_EQ(result.value, max_posit.value)
      << "Overflow did not saturate to maxpos: " << std::hex << "0x"
      << (int)result.value << " vs 0x" << (int)max_posit.value;

  // Test underflow with subtraction
  posit8 min_posit = posit8().minpos();
  posit8 neg_max = -max_posit;
  result = neg_max - max_posit;

  // Should saturate to -maxpos, not wrap around
  EXPECT_EQ(result.value, neg_max.value)
      << "Underflow did not saturate to -maxpos: " << std::hex << "0x"
      << (int)result.value << " vs 0x" << (int)neg_max.value;
}

// Test for subnormal values handling
TEST(Posit8Advanced, SubnormalHandling) {
  current_operation = "Subnormal Values";

  // Test with very small positive values
  posit8 min_positive = posit8().minpos();
  posit8 half_min = min_positive / posit8(2.0);

  // Should not be zero (posits don't have subnormals but have very small
  // values)
  EXPECT_NE(half_min.value, 0)
      << "Half of minpos became zero, expected a small positive value";

  // Test with very small negative values
  posit8 max_negative = posit8().minneg();
  posit8 half_max_neg = max_negative / posit8(2.0);

  // Should not be zero
  EXPECT_NE(half_max_neg.value, 0)
      << "Half of minneg became zero, expected a small negative value";
} 