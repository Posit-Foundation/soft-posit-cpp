#include "common_tests/test_utils.h"

// POSIT16 INCREMENT/DECREMENT TESTS

TEST(Posit16Operators, PreIncrement) {
  // Test regular values
  {
    posit16 p = 5.0;
    posit16 result = ++p;
    ASSERT_EQ(p, posit16(6.0));
    ASSERT_EQ(result, posit16(6.0)); // Pre-increment returns new value
  }

  // Test zero
  {
    posit16 p = 0.0;
    posit16 result = ++p;
    ASSERT_EQ(p, posit16(1.0));
    ASSERT_EQ(result, posit16(1.0));
  }

  // Test negative value
  {
    posit16 p = -3.0;
    posit16 result = ++p;
    ASSERT_EQ(p, posit16(-2.0));
    ASSERT_EQ(result, posit16(-2.0));
  }
}

TEST(Posit16Operators, PreDecrement) {
  // Test regular values
  {
    posit16 p = 5.0;
    posit16 result = --p;
    ASSERT_EQ(p, posit16(4.0));
    ASSERT_EQ(result, posit16(4.0)); // Pre-decrement returns new value
  }

  // Test near zero
  {
    posit16 p = 1.0;
    posit16 result = --p;
    ASSERT_EQ(p, posit16(0.0));
    ASSERT_EQ(result, posit16(0.0));
  }

  // Test zero (going negative)
  {
    posit16 p = 0.0;
    posit16 result = --p;
    ASSERT_EQ(p, posit16(-1.0));
    ASSERT_EQ(result, posit16(-1.0));
  }

  // Test negative value
  {
    posit16 p = -3.0;
    posit16 result = --p;
    ASSERT_EQ(p, posit16(-4.0));
    ASSERT_EQ(result, posit16(-4.0));
  }
}