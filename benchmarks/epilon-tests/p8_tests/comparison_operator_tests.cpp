#include "common_tests/test_utils.h"

// POSIT8 COMPARISON OPERATOR TESTS

TEST(Posit8Comparison, LessThan) {
  double a = 5.0;
  double b = 10.0;
  posit8 pa = a;
  posit8 pb = b;

  ASSERT_TRUE(pa < pb);
  ASSERT_FALSE(pb < pa);

  // Test with negative numbers
  pa = -a;
  pb = -b;
  ASSERT_FALSE(pa < pb);
  ASSERT_TRUE(pb < pa);
}

TEST(Posit8Comparison, LessThanEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit8 pa = a;
  posit8 pb = b;
  posit8 pc = c;

  ASSERT_TRUE(pa <= pb);
  ASSERT_TRUE(pa <= pc);
  ASSERT_FALSE(pc <= pa);
}

TEST(Posit8Comparison, Equal) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit8 pa = a;
  posit8 pb = b;
  posit8 pc = c;

  ASSERT_TRUE(pa == pb);
  ASSERT_FALSE(pa == pc);
}

TEST(Posit8Comparison, NotEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit8 pa = a;
  posit8 pb = b;
  posit8 pc = c;

  ASSERT_FALSE(pa != pb);
  ASSERT_TRUE(pa != pc);
}

TEST(Posit8Comparison, GreaterThan) {
  double a = 10.0;
  double b = 5.0;
  posit8 pa = a;
  posit8 pb = b;

  ASSERT_TRUE(pa > pb);
  ASSERT_FALSE(pb > pa);

  // Test with negative numbers
  pa = -a;
  pb = -b;
  ASSERT_FALSE(pa > pb);
  ASSERT_TRUE(pb > pa);
}

TEST(Posit8Comparison, GreaterThanEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit8 pa = a;
  posit8 pb = b;
  posit8 pc = c;

  ASSERT_TRUE(pa >= pb);
  ASSERT_TRUE(pc >= pa);
  ASSERT_FALSE(pa >= pc);
} 