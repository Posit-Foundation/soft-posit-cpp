#include "common_tests/test_utils.h"
#include <cmath>
#include <random>

// Test for p8_ceil function implementation
TEST(Posit8MathFunctions, CeilFunction) {
  current_operation = "Ceiling Function";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    // Calculate ceiling using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.ceil();
    double f_result = std::ceil(f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed ceil: ceil(" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for ceiling function
TEST(Posit8MathFunctions, CeilSpecificValues) {
  current_operation = "Ceiling Function Special Cases";

  // ceil(1.0) = 1.0
  posit8 p_one = posit8(1.0);
  posit8 result_one = p_one.ceil();
  ASSERT_EQ(result_one.value, p_one.value)
      << "ceil(1.0) = " << result_one.toDouble() << " but expected 1.0";

  // ceil(1.5) = 2.0
  posit8 p_one_point_five = posit8(1.5);
  posit8 p_two = posit8(2.0);
  posit8 result_one_five = p_one_point_five.ceil();
  ASSERT_EQ(result_one_five.value, p_two.value)
      << "ceil(1.5) = " << result_one_five.toDouble() << " but expected 2.0";

  // ceil(-1.5) = -1.0
  posit8 p_neg_one_point_five = posit8(-1.5);
  posit8 p_neg_one = posit8(-1.0);
  posit8 result_neg_one_five = p_neg_one_point_five.ceil();
  ASSERT_EQ(result_neg_one_five.value, p_neg_one.value)
      << "ceil(-1.5) = " << result_neg_one_five.toDouble() << " but expected -1.0";

  // ceil(0.0) = 0.0
  posit8 p_zero = posit8(0.0);
  posit8 result_zero = p_zero.ceil();
  ASSERT_EQ(result_zero.value, p_zero.value)
      << "ceil(0.0) = " << result_zero.toDouble() << " but expected 0.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.ceil();
  ASSERT_TRUE(result_nar.isNaR())
      << "ceil(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p8_floor function implementation
TEST(Posit8MathFunctions, FloorFunction) {
  current_operation = "Floor Function";

  // Create a distribution that generates values across the full posit8 range
  std::uniform_int_distribution<uint8_t> full_dist(0, 0xFF);

  for (int i = 0; i < NTESTS8; i++) {
    posit8 p_a;
    p_a.value = full_dist(gen);

    // Skip NaR values for input
    if (p_a.isNaR()) {
      continue;
    }

    // Calculate floor using both methods
    double f_a = p_a.toDouble();
    posit8 p_result = p_a.floor();
    double f_result = std::floor(f_a);
    posit8 p_expected = posit8(f_result);

    // Allow small differences due to rounding
    ASSERT_TRUE(
        double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-12, 1e-1))
        << "Failed floor: floor(" << p_a.toDouble() << ") = " << p_result.toDouble()
        << " but expected " << p_expected.toDouble() << " (hex: 0x" << std::hex
        << (int)p_a.value << " -> 0x" << (int)p_result.value << ", expected 0x"
        << (int)p_expected.value << ")";
  }
}

// Test specific known values for floor function
TEST(Posit8MathFunctions, FloorSpecificValues) {
  current_operation = "Floor Function Special Cases";

  // floor(1.0) = 1.0
  posit8 p_one = posit8(1.0);
  posit8 result_one = p_one.floor();
  ASSERT_EQ(result_one.value, p_one.value)
      << "floor(1.0) = " << result_one.toDouble() << " but expected 1.0";

  // floor(1.5) = 1.0
  posit8 p_one_point_five = posit8(1.5);
  posit8 result_one_five = p_one_point_five.floor();
  ASSERT_EQ(result_one_five.value, p_one.value)
      << "floor(1.5) = " << result_one_five.toDouble() << " but expected 1.0";

  // floor(-1.5) = -2.0
  posit8 p_neg_one_point_five = posit8(-1.5);
  posit8 p_neg_two = posit8(-2.0);
  posit8 result_neg_one_five = p_neg_one_point_five.floor();
  ASSERT_EQ(result_neg_one_five.value, p_neg_two.value)
      << "floor(-1.5) = " << result_neg_one_five.toDouble() << " but expected -2.0";

  // floor(0.0) = 0.0
  posit8 p_zero = posit8(0.0);
  posit8 result_zero = p_zero.floor();
  ASSERT_EQ(result_zero.value, p_zero.value)
      << "floor(0.0) = " << result_zero.toDouble() << " but expected 0.0";

  // NaR input should result in NaR
  posit8 p_nar;
  p_nar.toNaR();
  posit8 result_nar = p_nar.floor();
  ASSERT_TRUE(result_nar.isNaR())
      << "floor(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test the relationship between ceil and floor
TEST(Posit8MathFunctions, CeilFloorRelationship) {
  current_operation = "Ceiling and Floor Relationship";

  // Create a distribution that generates non-integer values
  std::uniform_real_distribution<double> frac_dist(-10.0, 10.0);

  for (int i = 0; i < NTESTS8 / 10; i++) {
    // Generate a non-integer value
    double f_a = frac_dist(gen);
    // Make sure it's not an integer by adding a non-integer part
    f_a += 0.3;
    
    posit8 p_a = posit8(f_a);
    
    // Calculate ceil(x) and floor(x)
    posit8 ceil_result = p_a.ceil();
    posit8 floor_result = p_a.floor();
    
    // For non-integer values, ceil(x) - floor(x) should be 1
    // For integer values, ceil(x) - floor(x) should be 0
    double difference = ceil_result.toDouble() - floor_result.toDouble();
    
    // If the original value was close to an integer due to posit precision,
    // the difference might be 0
    bool is_close_to_integer = std::abs(p_a.toDouble() - std::round(p_a.toDouble())) < 1e-3;
    
    if (is_close_to_integer) {
      ASSERT_TRUE(difference == 0.0)
          << "For integer-like value " << p_a.toDouble() 
          << ", ceil(" << p_a.toDouble() << ") - floor(" << p_a.toDouble() 
          << ") = " << difference << " but expected 0";
    } else {
      ASSERT_TRUE(difference == 1.0)
          << "For non-integer value " << p_a.toDouble() 
          << ", ceil(" << p_a.toDouble() << ") - floor(" << p_a.toDouble() 
          << ") = " << difference << " but expected 1";
    }
  }
} 