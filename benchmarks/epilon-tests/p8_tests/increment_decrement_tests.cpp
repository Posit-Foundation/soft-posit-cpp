#include "common_tests/test_utils.h"

// POSIT8 INCREMENT/DECREMENT TESTS

TEST(Posit8Operators, PreIncrement) {
  // Test regular values
  {
    posit8 p = 5.0;
    posit8 result = ++p;
    ASSERT_EQ(p, posit8(6.0));
    ASSERT_EQ(result, posit8(6.0)); // Pre-increment returns new value
  }

  // Test zero
  {
    posit8 p = 0.0;
    posit8 result = ++p;
    ASSERT_EQ(p, posit8(1.0));
    ASSERT_EQ(result, posit8(1.0));
  }

  // Test negative value
  {
    posit8 p = -3.0;
    posit8 result = ++p;
    ASSERT_EQ(p, posit8(-2.0));
    ASSERT_EQ(result, posit8(-2.0));
  }
}

TEST(Posit8Operators, PreDecrement) {
  // Test regular values
  {
    posit8 p = 5.0;
    posit8 result = --p;
    ASSERT_EQ(p, posit8(4.0));
    ASSERT_EQ(result, posit8(4.0)); // Pre-decrement returns new value
  }

  // Test near zero
  {
    posit8 p = 1.0;
    posit8 result = --p;
    ASSERT_EQ(p, posit8(0.0));
    ASSERT_EQ(result, posit8(0.0));
  }

  // Test zero (going negative)
  {
    posit8 p = 0.0;
    posit8 result = --p;
    ASSERT_EQ(p, posit8(-1.0));
    ASSERT_EQ(result, posit8(-1.0));
  }

  // Test negative value
  {
    posit8 p = -3.0;
    posit8 result = --p;
    ASSERT_EQ(p, posit8(-4.0));
    ASSERT_EQ(result, posit8(-4.0));
  }
}