#include "common_tests/test_utils.h"

// POSIT8 BIT OPERATIONS TESTS

TEST(Posit8BitOperations, BitwiseNot) {
  posit8 p;
  p.value = 0x55;
  posit8 not_p = ~p;
  ASSERT_EQ(not_p.value, 0xAA);
}

TEST(Posit8BitOperations, BitwiseAnd) {
  posit8 a, b;
  a.value = 0x55;
  b.value = 0x33;
  posit8 result = a & b;
  ASSERT_EQ(result.value, 0x11);
}

TEST(Posit8BitOperations, BitwiseOr) {
  posit8 a, b;
  a.value = 0x55;
  b.value = 0x33;
  posit8 result = a | b;
  ASSERT_EQ(result.value, 0x77);
}

TEST(Posit8BitOperations, BitwiseXor) {
  posit8 a, b;
  a.value = 0x55;
  b.value = 0x33;
  posit8 result = a ^ b;
  ASSERT_EQ(result.value, 0x66);
}

TEST(Posit8BitOperations, LeftShift) {
  posit8 p;
  p.value = 0x12;
  posit8 result = p << 4;
  ASSERT_EQ(result.value, 0x20);
}

TEST(Posit8BitOperations, RightShift) {
  posit8 p;
  p.value = 0x12;
  posit8 result = p >> 4;
  ASSERT_EQ(result.value, 0x01);
} 