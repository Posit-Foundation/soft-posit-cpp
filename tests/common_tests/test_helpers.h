

// tests/common_tests/test_helpers.h
#ifndef SOFTPOSIT_TEST_HELPERS_H
#define SOFTPOSIT_TEST_HELPERS_H

#include <cmath>
#include <functional>
#include "softposit_cpp.h"

namespace softposit_test
{

    // Helper function to compare doubles with tolerance
    inline bool double_eq(double a, double b, double epsilon = 1e-2)
    {
        return std::abs(a - b) < epsilon || std::abs(a - b) / (std::abs(a) + std::abs(b) + 1e-10) < epsilon;
    }

    // ULP difference functions for each posit size
    inline int8_t ulp_p8(posit8 x, posit8 y)
    {
        int8_t xi = x.value;
        int8_t yi = y.value;
        return std::abs(xi - yi);
    }

    inline int16_t ulp_p16(posit16 x, posit16 y)
    {
        int16_t xi = x.value;
        int16_t yi = y.value;
        return std::abs(xi - yi);
    }

    inline int32_t ulp_p32(posit32 x, posit32 y)
    {
        int32_t xi = x.value;
        int32_t yi = y.value;
        return std::abs(xi - yi);
    }

    // Generic test function for exact operations (templated for different posit sizes)
    template <typename PositT, typename DoubleOpT>
    void test_exact(std::function<PositT(PositT, PositT)> posit_op,
                    DoubleOpT double_op,
                    int num_tests,
                    std::function<PositT()> gen_random_posit)
    {
        for (int i = 0; i < num_tests; i++)
        {
            // Generate random posit values
            PositT p_a = gen_random_posit();
            PositT p_b = gen_random_posit();

            // Convert to double
            double f_a = p_a.toDouble();
            double f_b = p_b.toDouble();

            // Calculate results
            PositT result = posit_op(p_a, p_b);
            double expected_double = double_op(f_a, f_b);
            PositT expected = expected_double;

            // Compare results
            ASSERT_EQ(result.value, expected.value)
                << "Input: (" << f_a << ", " << f_b << ")"
                << "\nResult: " << result.toDouble()
                << "\nExpected: " << expected_double;
        }
    }

} // namespace softposit_test

#endif // SOFTPOSIT_TEST_HELPERS_H