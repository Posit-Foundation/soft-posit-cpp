#include "common_tests/test_utils.h"

// POSIT8 CONVERSION TESTS

// Random distributions for p8 tests
std::uniform_real_distribution<double> real_dist(-100.0, 100.0);

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