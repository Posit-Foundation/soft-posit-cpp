#include "common_tests/test_utils.h"

// POSIT32 COMPARISON OPERATOR TESTS

TEST(Posit32Comparison, LessThan) {
  double a = 5.0;
  double b = 10.0;
  posit32 pa = a;
  posit32 pb = b;

  ASSERT_TRUE(pa < pb);
  ASSERT_FALSE(pb < pa);

  // Test with negative numbers
  pa = -a;
  pb = -b;
  ASSERT_FALSE(pa < pb);
  ASSERT_TRUE(pb < pa);
}

TEST(Posit32Comparison, LessThanEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit32 pa = a;
  posit32 pb = b;
  posit32 pc = c;

  ASSERT_TRUE(pa <= pb);
  ASSERT_TRUE(pa <= pc);
  ASSERT_FALSE(pc <= pa);
}

TEST(Posit32Comparison, Equal) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit32 pa = a;
  posit32 pb = b;
  posit32 pc = c;

  ASSERT_TRUE(pa == pb);
  ASSERT_FALSE(pa == pc);
}

TEST(Posit32Comparison, NotEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit32 pa = a;
  posit32 pb = b;
  posit32 pc = c;

  ASSERT_FALSE(pa != pb);
  ASSERT_TRUE(pa != pc);
}

TEST(Posit32Comparison, GreaterThan) {
  double a = 10.0;
  double b = 5.0;
  posit32 pa = a;
  posit32 pb = b;

  ASSERT_TRUE(pa > pb);
  ASSERT_FALSE(pb > pa);

  // Test with negative numbers
  pa = -a;
  pb = -b;
  ASSERT_FALSE(pa > pb);
  ASSERT_TRUE(pb > pa);
}

TEST(Posit32Comparison, GreaterThanEqual) {
  double a = 5.0;
  double b = 5.0;
  double c = 10.0;
  posit32 pa = a;
  posit32 pb = b;
  posit32 pc = c;

  ASSERT_TRUE(pa >= pb);
  ASSERT_TRUE(pc >= pa);
  ASSERT_FALSE(pa >= pc);
}