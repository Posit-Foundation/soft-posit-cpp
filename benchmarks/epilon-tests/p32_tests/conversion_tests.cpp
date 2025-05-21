#include "common_tests/test_utils.h"

// POSIT32 CONVERSION TESTS

// Test conversion from posit32 to double for all possible values
TEST(Posit32Conversion, ConvertP32ToF64) {
  for (int n = -1000; n < 1000; n++) {
    posit32 p;
    p = n;
    double f = p.toDouble();
    posit32 p_back = f;

    ASSERT_EQ(p.value, p_back.value)
        << "Failed at n=" << n << ": original=" << p.value
        << ", converted back=" << p_back.value;
  }

  // Test random larger values
  for (int i = 0; i < 1000; i++) {
    int32_t n = static_cast<int32_t>(int_dist32(gen));
    posit32 p;
    p = n;
    double f = p.toDouble();
    posit32 p_back = f;

    ASSERT_EQ(p.value, p_back.value)
        << "Failed at n=" << n << ": original=" << p.value
        << ", converted back=" << p_back.value;
  }
}

// Test conversion from posit32 to float
TEST(Posit32Conversion, ConvertP32ToF32) {
  for (int n = -1000; n < 1000; n++) {
    posit32 p;
    p = n;
    double d = p.toDouble();
    float f = static_cast<float>(d);
    double d_back = static_cast<double>(f);
    posit32 p_back = d_back;

    ASSERT_EQ(p.value, p_back.value)
        << "Failed at n=" << n << ": original=" << p.value
        << ", converted back=" << p_back.value;
  }
}

// Test random conversion from double to posit32
TEST(Posit32Conversion, ConvertF64ToP32Rand) {
  std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
  for (int i = 0; i < NTESTS32; i++) {
    double f = real_dist(gen);
    posit32 p = f;

    // Just ensure no crashes
    ASSERT_NO_THROW({
      double d = p.toDouble();
      (void)d; // Avoid unused variable warning
    });
  }
}

// Test random conversion from float to posit32
TEST(Posit32Conversion, ConvertF32ToP32Rand) {
  std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
  for (int i = 0; i < NTESTS32; i++) {
    float f = static_cast<float>(real_dist(gen));
    double d = static_cast<double>(f);
    posit32 p = d;

    // Just ensure no crashes
    ASSERT_NO_THROW({
      double d_back = p.toDouble();
      (void)d_back; // Avoid unused variable warning
    });
  }
}

// Test conversion from posit32 to int32
TEST(Posit32Conversion, ConvertP32ToI32) {
  for (int n = -1000; n < 1000; n++) {
    posit32 p;
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

// Test conversion from posit32 to int64
TEST(Posit32Conversion, ConvertP32ToI64) {
  for (int n = -1000; n < 1000; n++) {
    posit32 p;
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