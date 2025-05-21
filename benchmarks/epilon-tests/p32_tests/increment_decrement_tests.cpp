#include "common_tests/test_utils.h"

// POSIT32 INCREMENT/DECREMENT TESTS

TEST(Posit32Operators, PreIncrement) {
  // Test regular values
  {
    posit32 p = 5.0;
    posit32 result = ++p;
    ASSERT_EQ(p, posit32(6.0));
    ASSERT_EQ(result, posit32(6.0)); // Pre-increment returns new value
  }

  // Test zero
  {
    posit32 p = 0.0;
    posit32 result = ++p;
    ASSERT_EQ(p, posit32(1.0));
    ASSERT_EQ(result, posit32(1.0));
  }

  // Test negative value
  {
    posit32 p = -3.0;
    posit32 result = ++p;
    ASSERT_EQ(p, posit32(-2.0));
    ASSERT_EQ(result, posit32(-2.0));
  }
}

TEST(Posit32Operators, PreDecrement) {
  // Test regular values
  {
    posit32 p = 5.0;
    posit32 result = --p;
    ASSERT_EQ(p, posit32(4.0));
    ASSERT_EQ(result, posit32(4.0)); // Pre-decrement returns new value
  }

  // Test near zero
  {
    posit32 p = 1.0;
    posit32 result = --p;
    ASSERT_EQ(p, posit32(0.0));
    ASSERT_EQ(result, posit32(0.0));
  }

  // Test zero (going negative)
  {
    posit32 p = 0.0;
    posit32 result = --p;
    ASSERT_EQ(p, posit32(-1.0));
    ASSERT_EQ(result, posit32(-1.0));
  }

  // Test negative value
  {
    posit32 p = -3.0;
    posit32 result = --p;
    ASSERT_EQ(p, posit32(-4.0));
    ASSERT_EQ(result, posit32(-4.0));
  }

  // Test near min value
  {
    posit32 p = -65536.0; // This might be near min for posit32
    posit32 original = p;
    posit32 result = --p;
    ASSERT_TRUE(p < original);
    ASSERT_EQ(result, p);
  }
}