#include "common_tests/test_utils.h"

// POSIT16 SPECIFIC VALUES TESTS

TEST(Posit16SpecificValues, Zero)
{
    posit16 p_zero(0);
    ASSERT_EQ(p_zero.value, 0);
    ASSERT_EQ(p_zero.toDouble(), 0.0);
}

TEST(Posit16SpecificValues, One)
{
    posit16 p_one(1);
    ASSERT_EQ(p_one.toDouble(), 1.0);
}

TEST(Posit16SpecificValues, MinMax)
{
    // Minimum value (most negative)
    posit16 p_min;
    p_min.value = 0x8001; // Sign bit set, smallest value after NaR
    
    // Maximum value (most positive)
    posit16 p_max;
    p_max.value = 0x7FFF; // Largest positive value
    
    ASSERT_LT(p_min.toDouble(), 0.0);
    ASSERT_GT(p_max.toDouble(), 0.0);
    
    // Verify min < max
    ASSERT_LT(p_min, p_max);
} 