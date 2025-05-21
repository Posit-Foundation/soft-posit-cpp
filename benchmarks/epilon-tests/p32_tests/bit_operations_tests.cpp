#include "common_tests/test_utils.h"

// POSIT32 BIT OPERATIONS TESTS

TEST(Posit32BitOperations, BitwiseNot) {
  posit32 p;
  p.value = 0x55555555;
  posit32 not_p = ~p;
  ASSERT_EQ(not_p.value, 0xAAAAAAAA);
}

TEST(Posit32BitOperations, BitwiseAnd) {
  posit32 a, b;
  a.value = 0x55555555;
  b.value = 0x33333333;
  posit32 result = a & b;
  ASSERT_EQ(result.value, 0x11111111);
}

TEST(Posit32BitOperations, BitwiseOr) {
  posit32 a, b;
  a.value = 0x55555555;
  b.value = 0x33333333;
  posit32 result = a | b;
  ASSERT_EQ(result.value, 0x77777777);
}

TEST(Posit32BitOperations, BitwiseXor) {
  posit32 a, b;
  a.value = 0x55555555;
  b.value = 0x33333333;
  posit32 result = a ^ b;
  ASSERT_EQ(result.value, 0x66666666);
}

TEST(Posit32BitOperations, LeftShift) {
  posit32 p;
  p.value = 0x12345678;
  posit32 result = p << 4;
  ASSERT_EQ(result.value, 0x23456780);
}

TEST(Posit32BitOperations, RightShift) {
  posit32 p;
  p.value = 0x12345678;
  posit32 result = p >> 4;
  ASSERT_EQ(result.value, 0x01234567);
}
