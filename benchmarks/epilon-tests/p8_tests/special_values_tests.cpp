#include "common_tests/test_utils.h"

// POSIT8 SPECIAL VALUES TESTS

TEST(Posit8Special, NaR) {
  // Test NaR detection
  posit8 p;
  p.toNaR();
  ASSERT_TRUE(p.isNaR());
  ASSERT_EQ(p.value, 0x80);

  // Test converting NaR to double (should not crash)
  ASSERT_NO_THROW({
    double d = p.toDouble();
    (void)d; // Avoid unused variable warning
  });
} 