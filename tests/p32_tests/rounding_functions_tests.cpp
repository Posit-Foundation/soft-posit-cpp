#include <cmath>
#include <random>

#include "common_tests/test_utils.h"

// Test for p32_ceil function implementation
TEST(Posit32MathFunctions, CeilFunction)
{
    // Create a distribution that generates values across the full posit32 range
    std::uniform_int_distribution<uint32_t> full_dist(0, 0xFFFFFFFF);

    for (int i = 0; i < NTESTS32; i++)
    {
        posit32 p_a;
        p_a.value = full_dist(gen);

        // Skip NaR values for input
        if (p_a.isNaR())
        {
            continue;
        }

        // Calculate ceiling using both methods
        double  f_a        = p_a.toDouble();
        posit32 p_result   = p_a.ceil();
        double  f_result   = std::ceil(f_a);
        posit32 p_expected = posit32(f_result);

        // Allow small differences due to rounding
        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-3))
            << "Failed ceil: ceil(" << f_a << ") = " << p_result.toDouble() << " but expected "
            << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value << " -> 0x"
            << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for ceiling function
TEST(Posit32MathFunctions, CeilSpecificValues)
{
    // ceil(1.0) = 1.0
    posit32 p_one      = posit32(1.0);
    posit32 result_one = p_one.ceil();
    ASSERT_EQ(result_one.value, p_one.value)
        << "ceil(1.0) = " << result_one.toDouble() << " but expected 1.0";

    // ceil(1.5) = 2.0
    posit32 p_one_point_five = posit32(1.5);
    posit32 p_two            = posit32(2.0);
    posit32 result_one_five  = p_one_point_five.ceil();
    ASSERT_EQ(result_one_five.value, p_two.value)
        << "ceil(1.5) = " << result_one_five.toDouble() << " but expected 2.0";

    // ceil(-1.5) = -1.0
    posit32 p_neg_one_point_five = posit32(-1.5);
    posit32 p_neg_one            = posit32(-1.0);
    posit32 result_neg_one_five  = p_neg_one_point_five.ceil();
    ASSERT_EQ(result_neg_one_five.value, p_neg_one.value)
        << "ceil(-1.5) = " << result_neg_one_five.toDouble() << " but expected -1.0";

    // ceil(0.0) = 0.0
    posit32 p_zero      = posit32(0.0);
    posit32 result_zero = p_zero.ceil();
    ASSERT_EQ(result_zero.value, p_zero.value)
        << "ceil(0.0) = " << result_zero.toDouble() << " but expected 0.0";

    // NaR input should result in NaR
    posit32 p_nar;
    p_nar.toNaR();
    posit32 result_nar = p_nar.ceil();
    ASSERT_TRUE(result_nar.isNaR())
        << "ceil(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test for p32_floor function implementation
TEST(Posit32MathFunctions, FloorFunction)
{
    // Create a distribution that generates values across the full posit32 range
    std::uniform_int_distribution<uint32_t> full_dist(0, 0xFFFFFFFF);

    for (int i = 0; i < NTESTS32; i++)
    {
        posit32 p_a;
        p_a.value = full_dist(gen);

        // Skip NaR values for input
        if (p_a.isNaR())
        {
            continue;
        }

        // Calculate floor using both methods
        double  f_a        = p_a.toDouble();
        posit32 p_result   = p_a.floor();
        double  f_result   = std::floor(f_a);
        posit32 p_expected = posit32(f_result);

        // Allow small differences due to rounding
        ASSERT_TRUE(double_eq(p_result.toDouble(), p_expected.toDouble(), 1e-3))
            << "Failed floor: floor(" << f_a << ") = " << p_result.toDouble() << " but expected "
            << p_expected.toDouble() << " (hex: 0x" << std::hex << p_a.value << " -> 0x"
            << p_result.value << ", expected 0x" << p_expected.value << ")";
    }
}

// Test specific known values for floor function
TEST(Posit32MathFunctions, FloorSpecificValues)
{
    // floor(1.0) = 1.0
    posit32 p_one      = posit32(1.0);
    posit32 result_one = p_one.floor();
    ASSERT_EQ(result_one.value, p_one.value)
        << "floor(1.0) = " << result_one.toDouble() << " but expected 1.0";

    // floor(1.5) = 1.0
    posit32 p_one_point_five = posit32(1.5);
    posit32 result_one_five  = p_one_point_five.floor();
    ASSERT_EQ(result_one_five.value, p_one.value)
        << "floor(1.5) = " << result_one_five.toDouble() << " but expected 1.0";

    // floor(-1.5) = -2.0
    posit32 p_neg_one_point_five = posit32(-1.5);
    posit32 p_neg_two            = posit32(-2.0);
    posit32 result_neg_one_five  = p_neg_one_point_five.floor();
    ASSERT_EQ(result_neg_one_five.value, p_neg_two.value)
        << "floor(-1.5) = " << result_neg_one_five.toDouble() << " but expected -2.0";

    // floor(0.0) = 0.0
    posit32 p_zero      = posit32(0.0);
    posit32 result_zero = p_zero.floor();
    ASSERT_EQ(result_zero.value, p_zero.value)
        << "floor(0.0) = " << result_zero.toDouble() << " but expected 0.0";

    // NaR input should result in NaR
    posit32 p_nar;
    p_nar.toNaR();
    posit32 result_nar = p_nar.floor();
    ASSERT_TRUE(result_nar.isNaR())
        << "floor(NaR) = " << result_nar.toDouble() << " but expected NaR";
}

// Test the relationship between ceil and floor
TEST(Posit32MathFunctions, CeilFloorRelationship)
{
    // Create a distribution that generates non-integer values
    std::uniform_real_distribution<double> frac_dist(-100.0, 100.0);

    for (int i = 0; i < NTESTS32 / 10; i++)
    {
        // Generate a random non-integer value
        double f_a = frac_dist(gen);
        // Ensure it's not an integer by adding fractional component
        f_a += 0.37;

        posit32 p_a = posit32(f_a);

        // Calculate ceil(x) and floor(x)
        posit32 ceil_result  = p_a.ceil();
        posit32 floor_result = p_a.floor();

        double difference = ceil_result.toDouble() - floor_result.toDouble();

        bool is_close_to_integer = std::abs(p_a.toDouble() - std::round(p_a.toDouble())) < 1e-3;

        if (is_close_to_integer)
        {
            ASSERT_TRUE(difference == 0.0)
                << "For integer-like value " << p_a.toDouble() << ", ceil(" << p_a.toDouble()
                << ") - floor(" << p_a.toDouble() << ") = " << difference << " but expected 0";
        }
        else
        {
            ASSERT_TRUE(difference == 1.0)
                << "For non-integer value " << p_a.toDouble() << ", ceil(" << p_a.toDouble()
                << ") - floor(" << p_a.toDouble() << ") = " << difference << " but expected 1";
        }
    }
}