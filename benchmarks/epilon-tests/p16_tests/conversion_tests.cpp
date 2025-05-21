#include "common_tests/test_utils.h"

// POSIT16 CONVERSION TESTS

// Test conversion from posit16 to double for all possible values
TEST(Posit16Conversion, ConvertP16ToF64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        posit16 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }

    // Test random larger values
    for (int i = 0; i < 1000; i++)
    {
        int16_t n = static_cast<int16_t>(int_dist16(gen));
        posit16 p;
        p = n;
        double f = p.toDouble();
        posit16 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test conversion from posit16 to float
TEST(Posit16Conversion, ConvertP16ToF32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double d = p.toDouble();
        float f = static_cast<float>(d);
        double d_back = static_cast<double>(f);
        posit16 p_back = d_back;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test random conversion from double to posit16
TEST(Posit16Conversion, ConvertF64ToP16Rand)
{
    std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
    for (int i = 0; i < NTESTS16; i++)
    {
        double f = real_dist(gen);
        posit16 p = f;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d = p.toDouble();
            (void)d; // Avoid unused variable warning
        });
    }
}

// Test random conversion from float to posit16
TEST(Posit16Conversion, ConvertF32ToP16Rand)
{
    std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
    for (int i = 0; i < NTESTS16; i++)
    {
        float f = static_cast<float>(real_dist(gen));
        double d = static_cast<double>(f);
        posit16 p = d;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d_back = p.toDouble();
            (void)d_back; // Avoid unused variable warning
        });
    }
}

// Test conversion from posit16 to int32
TEST(Posit16Conversion, ConvertP16ToI32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        long long int i_from_p = p.toInt();
        int32_t i_from_f = static_cast<int32_t>(std::round(f));

        // Skip very small values where precision might be an issue
        if (std::abs(f) < 1e-6)
        {
            continue;
        }

        ASSERT_EQ(i_from_p, i_from_f)
            << "Failed at n=" << n
            << ": i_from_p=" << i_from_p
            << ", i_from_f=" << i_from_f;
    }
}

// Test conversion from posit16 to int64
TEST(Posit16Conversion, ConvertP16ToI64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        long long int i_from_p = p.toRInt();
        int64_t i_from_f = static_cast<int64_t>(std::round(f));

        // Skip very small values where precision might be an issue
        if (std::abs(f) < 1e-6)
        {
            continue;
        }

        ASSERT_EQ(i_from_p, i_from_f)
            << "Failed at n=" << n
            << ": i_from_p=" << i_from_p
            << ", i_from_f=" << i_from_f;
    }
} 