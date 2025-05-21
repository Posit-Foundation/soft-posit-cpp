#include "common_tests/test_utils.h"

// POSIT16 BIT OPERATIONS TESTS

TEST(Posit16BitOperations, BitwiseNot) {
  posit16 p;
  p.value = 0x5555;
  posit16 not_p = ~p;
  ASSERT_EQ(not_p.value, 0xAAAA);
}

TEST(Posit16BitOperations, BitwiseAnd) {
  posit16 a, b;
  a.value = 0x5555;
  b.value = 0x3333;
  posit16 result = a & b;
  ASSERT_EQ(result.value, 0x1111);
}

TEST(Posit16BitOperations, BitwiseOr) {
  posit16 a, b;
  a.value = 0x5555;
  b.value = 0x3333;
  posit16 result = a | b;
  ASSERT_EQ(result.value, 0x7777);
}

TEST(Posit16BitOperations, BitwiseXor) {
  posit16 a, b;
  a.value = 0x5555;
  b.value = 0x3333;
  posit16 result = a ^ b;
  ASSERT_EQ(result.value, 0x6666);
}

TEST(Posit16BitOperations, LeftShift) {
  posit16 p;
  p.value = 0x1234;
  posit16 result = p << 4;
  ASSERT_EQ(result.value, 0x2340);
}

TEST(Posit16BitOperations, RightShift) {
  posit16 p;
  p.value = 0x1234;
  posit16 result = p >> 4;
  ASSERT_EQ(result.value, 0x0123);
}
