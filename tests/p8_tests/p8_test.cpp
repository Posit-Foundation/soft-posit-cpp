#include "common_tests/test_utils.h"

// Random distributions for p8 tests
std::uniform_int_distribution<int8_t> int_dist8(INT8_MIN, INT8_MAX);
std::uniform_real_distribution<double> real_dist(-100.0, 100.0);

//========================================================================================
// POSIT8 TESTS
//========================================================================================

// Test case for addition
TEST(Posit8Arithmetic, Add) {
  current_operation = "Addition";
  test_exact<posit8>(
      NTESTS8,
      // Posit operation
      [](posit8 a, posit8 b) { return a + b; },
      // Equivalent double operation
      [](double a, double b) { return a + b; },
      // Random generator
      []() { return int_dist8(gen); });
}

// Test case for subtraction
TEST(Posit8Arithmetic, Sub) {
  current_operation = "Subtraction";
  test_exact<posit8>(
      NTESTS8,
      // Posit operation
      [](posit8 a, posit8 b) { return a - b; },
      // Equivalent double operation
      [](double a, double b) { return a - b; },
      // Random generator
      []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test case for multiplication
TEST(Posit8Arithmetic, Mul) {
  current_operation = "Multiplication";
  test_exact<posit8>(
      NTESTS8,
      // Posit operation
      [](posit8 a, posit8 b) { return a * b; },
      // Equivalent double operation
      [](double a, double b) { return a * b; },
      // Random generator
      []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test case for division
TEST(Posit8Arithmetic, Div) {
  current_operation = "Division";
  test_exact<posit8>(
      NTESTS8,
      // Posit operation
      [](posit8 a, posit8 b) {
        // Skip division by zero
        if (b.value == 0 || b.isNaR()) {
          return posit8(0);
        }
        return a / b;
      },
      // Equivalent double operation
      [](double a, double b) {
        if (b == 0 || std::isnan(b) || std::isinf(b)) {
          return 0.0;
        }
        return a / b;
      },
      // Random generator
      []() { return static_cast<int8_t>(int_dist8(gen)); });
}

// Test square root operation
TEST(Posit8Arithmetic, Sqrt) {
  current_operation = "Square Root";

  for (int i = 0; i < NTESTS8; i++) {
    // Generate random posit value
    posit8 p_a;
    p_a.value = int_dist8(gen);

    double f_a = p_a.toDouble();

    // Skip negative values for sqrt and NaN values
    if (f_a < 0 || std::isnan(f_a) || p_a.isNaR())
      continue;

    posit8 p_result = p_a;
    p_result.sqrt();

    double f_result = std::sqrt(f_a);
    posit8 expected = f_result;

    ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
        << "sqrt(" << f_a << ") = " << f_result << " but got "
        << p_result.toDouble();
  }
}

// Test rounding operation
TEST(Posit8Arithmetic, Round) {
  current_operation = "Rounding";
  for (int i = 0; i < NTESTS8; i++) {
    // Generate random posit value
    posit8 p_a;
    p_a.value = int_dist8(gen);

    double f_a = p_a.toDouble();

    // Skip NaN values
    if (std::isnan(f_a) || p_a.isNaR())
      continue;

    posit8 p_result = p_a;
    p_result.rint();

    double f_result = std::round(f_a);
    posit8 expected = f_result;

    // Skip values at exactly 0.5 boundary like in Rust test
    if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10) {
      continue;
    }

    ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
        << "round(" << f_a << ") = " << f_result << " but got "
        << p_result.toDouble();
  }
}

// Test fused multiply-add operation
TEST(Posit8Arithmetic, MulAdd) {
  current_operation = "Fused Multiply-Add";
  for (int i = 0; i < NTESTS8; i++) {
    // Generate random posit values
    posit8 p_a, p_b, p_c;
    p_a.value = int_dist8(gen);
    p_b.value = int_dist8(gen);
    p_c.value = int_dist8(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    posit8 p_result = p_c.fma(p_a, p_b);
    double f_result = std::fma(f_a, f_b, f_c);
    posit8 expected = f_result;

    // Allow up to 1 ULP difference for posit8
    auto ulp_diff = ulp(p_result, expected);
    ASSERT_LE(ulp_diff, 1) << "fma(" << f_a << ", " << f_b << ", " << f_c
                           << ") = " << f_result << " but got "
                           << p_result.toDouble() << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

//========================================================================================
// QUIRE8 TESTS
//========================================================================================

// Test for quire_mul_add
TEST(Quire8Operations, MulAdd) {
  current_operation = "Quire Multiply-Add";
  for (int i = 0; i < NTESTS8; i++) {
    // Generate random posit values
    posit8 p_a, p_b, p_c;
    p_a.value = int_dist8(gen);
    p_b.value = int_dist8(gen);
    p_c.value = int_dist8(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire8 q;
    q.clr();

    // Perform operations (q += (p_a, p_b); q += p_c;)
    q.qma(p_a, p_b);
    q.qma(posit8(1.0), p_c);

    // Convert back to posit
    posit8 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(f_a, f_b, f_c);
    posit8 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

// Test for quire_mul_sub
TEST(Quire8Operations, MulSub) {
  current_operation = "Quire Multiply-Subtract";
  for (int i = 0; i < NTESTS8; i++) {
    // Generate random posit values
    posit8 p_a, p_b, p_c;
    p_a.value = int_dist8(gen);
    p_b.value = int_dist8(gen);
    p_c.value = int_dist8(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire8 q;
    q.clr();

    // Perform operations (q -= (p_a, p_b); q += p_c;)
    q.qms(p_a, p_b);
    q.qma(posit8(1.0), p_c);

    // Convert back to posit
    posit8 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(-f_a, f_b, f_c);
    posit8 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (-" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

//========================================================================================
// CONVERSION TESTS
//========================================================================================

// Test conversion from posit8 to double for all possible values
TEST(Posit8Conversion, ConvertP8ToF64) {
  // For posit8, we can test all possible values (256 combinations)
  for (int n = -128; n < 128; n++) {
    posit8 p;
    p.value = static_cast<uint8_t>(n);
    double f = p.toDouble();
    posit8 p_back = f;

    ASSERT_EQ(p.value, p_back.value)
        << "Failed at n=" << n << ": original=" << (int)p.value
        << ", converted back=" << (int)p_back.value;
  }
}

// Test conversion from posit8 to float
TEST(Posit8Conversion, ConvertP8ToF32) {
  for (int n = -128; n < 128; n++) {
    posit8 p;
    p.value = static_cast<uint8_t>(n);
    double d = p.toDouble();
    float f = static_cast<float>(d);
    double d_back = static_cast<double>(f);
    posit8 p_back = d_back;

    ASSERT_EQ(p.value, p_back.value)
        << "Failed at n=" << n << ": original=" << (int)p.value
        << ", converted back=" << (int)p_back.value;
  }
}

// Test random conversion from double to posit8
TEST(Posit8Conversion, ConvertF64ToP8Rand) {
  for (int i = 0; i < NTESTS8; i++) {
    double f = real_dist(gen);
    posit8 p = f;

    // Just ensure no crashes
    ASSERT_NO_THROW({
      double d = p.toDouble();
      (void)d; // Avoid unused variable warning
    });
  }
}

// Test random conversion from float to posit8
TEST(Posit8Conversion, ConvertF32ToP8Rand) {
  for (int i = 0; i < NTESTS8; i++) {
    float f = static_cast<float>(real_dist(gen));
    double d = static_cast<double>(f);
    posit8 p = d;

    // Just ensure no crashes
    ASSERT_NO_THROW({
      double d_back = p.toDouble();
      (void)d_back; // Avoid unused variable warning
    });
  }
}

// Test conversion from posit8 to int32
TEST(Posit8Conversion, ConvertP8ToI32) {
  for (int n = -128; n < 128; n++) {
    posit8 p;
    p = n;
    double f = p.toDouble();
    long long int i_from_p = p.toInt();
    int32_t i_from_f = static_cast<int32_t>(std::round(f));

    // Skip very small values where precision might be an issue
    if (std::abs(f) < 1e-6) {
      continue;
    }

    ASSERT_EQ(i_from_p, i_from_f)
        << "Failed at n=" << n << ": i_from_p=" << i_from_p
        << ", i_from_f=" << i_from_f;
  }
}

// Test conversion from posit8 to int64
TEST(Posit8Conversion, ConvertP8ToI64) {
  for (int n = -128; n < 128; n++) {
    posit8 p;
    p = n;
    double f = p.toDouble();
    long long int i_from_p = p.toRInt();
    int64_t i_from_f = static_cast<int64_t>(std::round(f));

    // Skip very small values where precision might be an issue
    if (std::abs(f) < 1e-6) {
      continue;
    }

    ASSERT_EQ(i_from_p, i_from_f)
        << "Failed at n=" << n << ": i_from_p=" << i_from_p
        << ", i_from_f=" << i_from_f;
  }
}

//========================================================================================
// SPECIAL VALUES TESTS
//========================================================================================

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

//========================================================================================
// COMPARISON OPERATOR TESTS
//========================================================================================

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

//========================================================================================
// INCREMENT/DECREMENT TESTS
//========================================================================================

TEST(Posit8Operators, Increment) {
  posit8 p = 5.0;
  posit8 original = p;
  ++p;

  ASSERT_TRUE(p > original);
}

TEST(Posit8Operators, Decrement) {
  posit8 p = 5.0;
  posit8 original = p;
  --p;

  ASSERT_TRUE(p < original);
}

//========================================================================================
// BIT OPERATIONS TESTS
//========================================================================================

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

//========================================================================================
// SPECIFIC VALUES TESTS
//========================================================================================

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

//========================================================================================
// ADVANCED TESTS
//========================================================================================

// Test that adding and then subtracting the same value returns the original
TEST(Posit8Advanced, AddSubCancel) {
  current_operation = "Add-Sub Cancellation";

  // Create a distribution that generates values in the range [-MAX/2, MAX/2]
  std::uniform_int_distribution<int8_t> safe_dist(INT8_MIN / 2, INT8_MAX / 2);

  for (int i = 0; i < NTESTS8 / 10; i++) { // Reduced iteration count
    posit8 p_a, p_b;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);

    // Skip NaN/NaR values
    if (p_a.isNaR() || p_b.isNaR() || std::isnan(p_a.toDouble()) ||
        std::isnan(p_b.toDouble())) {
      continue;
    }

    posit8 sum = p_a + p_b;
    posit8 original = sum - p_b;

    // Allow a slightly higher tolerance for posit8
    EXPECT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
        << "Failed: (" << p_a.toDouble() << " + " << p_b.toDouble() << ") - "
        << p_b.toDouble() << " = " << original.toDouble() << " but expected "
        << p_a.toDouble();
  }
}

// Test that multiplying and then dividing by the same value returns the
// original
TEST(Posit8Advanced, MulDivCancel) {
  current_operation = "Mul-Div Cancellation";

  // Create a distribution that generates values in the range [-MAX/2, MAX/2]
  std::uniform_int_distribution<int8_t> safe_dist(INT8_MIN / 2, INT8_MAX / 2);

  for (int i = 0; i < NTESTS8 / 10; i++) { // Reduced iteration count
    posit8 p_a, p_b;
    p_a.value = safe_dist(gen);
    p_b.value = safe_dist(gen);

    // Skip division by zero, NaR, NaN, or very small numbers
    if (p_b.value == 0 || p_b.isNaR() || p_a.isNaR() ||
        std::isnan(p_b.toDouble()) || std::isnan(p_a.toDouble()) ||
        std::abs(p_b.toDouble()) < 1e-6) {
      continue;
    }

    posit8 product = p_a * p_b;
    posit8 original = product / p_b;

    // Allow a slightly higher tolerance for posit8
    EXPECT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
        << "Failed: (" << p_a.toDouble() << " * " << p_b.toDouble() << ") / "
        << p_b.toDouble() << " = " << original.toDouble() << " but expected "
        << p_a.toDouble();
  }
}

// Test for overflow behavior
TEST(Posit8Advanced, OverflowHandling) {
  current_operation = "Overflow Handling";

  // Test overflow with addition
  posit8 max_posit = posit8().maxpos();
  posit8 result = max_posit + max_posit;

  // Should saturate to maxpos, not wrap around or become NaR
  EXPECT_EQ(result.value, max_posit.value)
      << "Overflow did not saturate to maxpos: " << std::hex << "0x"
      << (int)result.value << " vs 0x" << (int)max_posit.value;

  // Test underflow with subtraction
  posit8 min_posit = posit8().minpos();
  posit8 neg_max = -max_posit;
  result = neg_max - max_posit;

  // Should saturate to -maxpos, not wrap around
  EXPECT_EQ(result.value, neg_max.value)
      << "Underflow did not saturate to -maxpos: " << std::hex << "0x"
      << (int)result.value << " vs 0x" << (int)neg_max.value;
}

// Test for subnormal values handling
TEST(Posit8Advanced, SubnormalHandling) {
  current_operation = "Subnormal Values";

  // Test with very small positive values
  posit8 min_positive = posit8().minpos();
  posit8 half_min = min_positive / posit8(2.0);

  // Should not be zero (posits don't have subnormals but have very small
  // values)
  EXPECT_NE(half_min.value, 0)
      << "Half of minpos became zero, expected a small positive value";

  // Test with very small negative values
  posit8 max_negative = posit8().minneg();
  posit8 half_max_neg = max_negative / posit8(2.0);

  // Should not be zero
  EXPECT_NE(half_max_neg.value, 0)
      << "Half of minneg became zero, expected a small negative value";
}

// Main function
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Add the custom listener
  ::testing::TestEventListeners &listeners =
      ::testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new EpsilonStatisticsPrinter);

  return RUN_ALL_TESTS();
}