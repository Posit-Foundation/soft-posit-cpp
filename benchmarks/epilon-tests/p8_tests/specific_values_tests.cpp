#include "common_tests/test_utils.h"

// POSIT8 SPECIFIC VALUES TESTS

TEST(Posit8SpecificValues, Zero) {
  posit8 p;
  p.value = 0;
  ASSERT_EQ(p.toDouble(), 0.0);

  posit8 zero = 0.0;
  ASSERT_EQ(zero.value, 0);
}

TEST(Posit8SpecificValues, One) {
  posit8 one = 1.0;
  ASSERT_EQ(one.toDouble(), 1.0);

  // Check specific bit pattern for 1.0
  ASSERT_EQ(one.value, 0x40);
}

TEST(Posit8SpecificValues, NegativeOne) {
  posit8 neg_one = -1.0;
  ASSERT_EQ(neg_one.toDouble(), -1.0);

  // Check specific bit pattern for -1.0
  ASSERT_EQ(neg_one.value, 0xC0);
} 