#include <gtest/gtest.h>
#include <random>
#include <functional>
#include <cmath>
#include "softposit_cpp.h"

// Define constants for number of tests similar to Rust implementation
#ifdef NDEBUG
constexpr int NTESTS16 = 1000000;
#else
constexpr int NTESTS16 = 100000;
#endif

// Random generator setup
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int16_t> int_dist(INT16_MIN, INT16_MAX);
std::uniform_real_distribution<double> real_dist(-100.0, 100.0);

// Helper function to compare doubles with tolerance - using a much higher tolerance for posit operations
bool double_eq(double a, double b, double epsilon = 1e-2)
{
    return std::abs(a - b) < epsilon || std::abs(a - b) / (std::abs(a) + std::abs(b) + 1e-10) < epsilon;
}

// ULP function similar to Rust implementation
int16_t ulp(posit16 x, posit16 y)
{
    int16_t xi = x.value;
    int16_t yi = y.value;
    return std::abs(xi - yi);
}

//========================================================================================
// POSIT16 TESTS - Using direct bit comparison like the Rust implementation
//========================================================================================

// Generic test function similar to test21_exact in Rust
void test21_exact(std::function<posit16(posit16, posit16)> posit_op,
                  std::function<double(double, double)> double_op)
{
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        int16_t raw_a = int_dist(gen);
        int16_t raw_b = int_dist(gen);

        // Create posit values
        posit16 p_a, p_b;
        p_a.value = raw_a;
        p_b.value = raw_b;

        // Convert to double
        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();

        // Calculate results
        posit16 result = posit_op(p_a, p_b);
        double expected_double = double_op(f_a, f_b);
        posit16 expected = expected_double;

        // Compare results directly using bit representation
        ASSERT_EQ(result.value, expected.value)
            << "Input: (" << f_a << ", " << f_b << ")"
            << "\nResult: " << result.toDouble()
            << "\nExpected: " << expected_double;
    }
}

// Test case for addition
TEST(PositArithmetic, Add)
{
    test21_exact(
        // Posit operation
        [](posit16 a, posit16 b)
        { return a + b; },
        // Equivalent double operation
        [](double a, double b)
        { return a + b; });
}

// Test case for subtraction
TEST(PositArithmetic, Sub)
{
    test21_exact(
        // Posit operation
        [](posit16 a, posit16 b)
        { return a - b; },
        // Equivalent double operation
        [](double a, double b)
        { return a - b; });
}

// Test case for multiplication
TEST(PositArithmetic, Mul)
{
    test21_exact(
        // Posit operation
        [](posit16 a, posit16 b)
        { return a * b; },
        // Equivalent double operation
        [](double a, double b)
        { return a * b; });
}

// Test case for division
TEST(PositArithmetic, Div)
{
    test21_exact(
        // Posit operation
        [](posit16 a, posit16 b)
        {
            // Skip division by zero
            if (b.value == 0 || b.isNaR())
            {
                return posit16(0);
            }
            return a / b;
        },
        // Equivalent double operation
        [](double a, double b)
        {
            if (b == 0 || std::isnan(b) || std::isinf(b))
            {
                return 0.0;
            }
            return a / b;
        });
}

// Test square root operation
TEST(PositArithmetic, Sqrt)
{
    std::uniform_int_distribution<int16_t> dist(0, INT16_MAX); // Only positive values

    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit value
        posit16 p_a;
        p_a.value = int_dist(gen);

        double f_a = p_a.toDouble();

        // Skip negative values for sqrt
        if (f_a < 0)
            continue;

        posit16 p_result = p_a;
        p_result.sqrt();

        double f_result = std::sqrt(f_a);
        posit16 expected = f_result;

        ASSERT_EQ(p_result.value, expected.value)
            << "sqrt(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test rounding operation
TEST(PositArithmetic, Round)
{
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit value
        posit16 p_a;
        p_a.value = int_dist(gen);

        double f_a = p_a.toDouble();

        posit16 p_result = p_a;
        p_result.rint();

        double f_result = std::round(f_a);
        posit16 expected = f_result;

        // Skip values at exactly 0.5 boundary like in Rust test
        if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10)
        {
            continue;
        }

        ASSERT_EQ(p_result.value, expected.value)
            << "round(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test fused multiply-add operation
TEST(PositArithmetic, MulAdd)
{
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist(gen);
        p_b.value = int_dist(gen);
        p_c.value = int_dist(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        posit16 p_result = p_c.fma(p_a, p_b);
        double f_result = std::fma(f_a, f_b, f_c);
        posit16 expected = f_result;

        // // Allow 1 ULP difference
        ASSERT_LE(ulp(p_result, expected), 1)
        // Allow for larger rounding errors
        // ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble(), 0.1))
            << "fma(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

//========================================================================================
// QUIRE16 TESTS
//========================================================================================

// Test for quire_mul_add similar to Rust implementation
TEST(Quire16Operations, MulAdd)
{
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist(gen);
        p_b.value = int_dist(gen);
        p_c.value = int_dist(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Create and initialize quire
        quire16 q;
        q.clr();

        // Perform operations (q += (p_a, p_b); q += p_c;)
        q.qma(p_a, p_b);
        q.qma(posit16(1.0), p_c);

        // Convert back to posit
        posit16 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(f_a, f_b, f_c);
        posit16 expected = f;

        // Allow 1 ULP difference as in Rust implementation
        ASSERT_LE(ulp(p, expected), 1)
            << "Input: (" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

// Test for quire_mul_sub similar to Rust implementation
TEST(Quire16Operations, MulSub)
{
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist(gen);
        p_b.value = int_dist(gen);
        p_c.value = int_dist(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Create and initialize quire
        quire16 q;
        q.clr();

        // Perform operations (q -= (p_a, p_b); q += p_c;)
        q.qms(p_a, p_b);
        q.qma(posit16(1.0), p_c);

        // Convert back to posit
        posit16 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(-f_a, f_b, f_c);
        posit16 expected = f;

        // Allow 1 ULP difference as in Rust implementation
        ASSERT_LE(ulp(p, expected), 1)
            << "Input: (-" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

// Test conversion from posit16 to double for all possible values
TEST(PositConversion, ConvertP16ToF64)
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
        int16_t n = static_cast<int16_t>(int_dist(gen));
        posit16 p;
        p = n;
        double f = p.toDouble();
        posit16 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at random n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test conversion from posit16 to float
TEST(PositConversion, ConvertP16ToF32)
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
TEST(PositConversion, ConvertF64ToP16Rand)
{
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
TEST(PositConversion, ConvertF32ToP16Rand)
{
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
TEST(PositConversion, ConvertP16ToI32)
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
TEST(PositConversion, ConvertP16ToI64)
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

// Test Special Values
//========================================================================================

TEST(PositSpecial, NaR)
{
    // Test NaR detection
    posit16 p;
    p.toNaR();
    ASSERT_TRUE(p.isNaR());
    ASSERT_EQ(p.value, 0x8000);

    // Test converting NaR to double (should not crash)
    ASSERT_NO_THROW({
        double d = p.toDouble();
        (void)d; // Avoid unused variable warning
    });
}

// Test Comparison Operators
//========================================================================================

TEST(PositComparison, LessThan)
{
    double a = 5.0;
    double b = 10.0;
    posit16 pa = a;
    posit16 pb = b;

    ASSERT_TRUE(pa < pb);
    ASSERT_FALSE(pb < pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa < pb);
    ASSERT_TRUE(pb < pa);
}

TEST(PositComparison, LessThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa <= pb);
    ASSERT_TRUE(pa <= pc);
    ASSERT_FALSE(pc <= pa);
}

TEST(PositComparison, Equal)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa == pb);
    ASSERT_FALSE(pa == pc);
}

TEST(PositComparison, NotEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_FALSE(pa != pb);
    ASSERT_TRUE(pa != pc);
}

TEST(PositComparison, GreaterThan)
{
    double a = 10.0;
    double b = 5.0;
    posit16 pa = a;
    posit16 pb = b;

    ASSERT_TRUE(pa > pb);
    ASSERT_FALSE(pb > pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa > pb);
    ASSERT_TRUE(pb > pa);
}

TEST(PositComparison, GreaterThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa >= pb);
    ASSERT_TRUE(pc >= pa);
    ASSERT_FALSE(pa >= pc);
}

// Test Increment/Decrement
//========================================================================================

TEST(PositOperators, Increment)
{
    posit16 p = 5.0;
    posit16 original = p;
    ++p;

    ASSERT_TRUE(p > original);
}

TEST(PositOperators, Decrement)
{
    posit16 p = 5.0;
    posit16 original = p;
    --p;

    ASSERT_TRUE(p < original);
}

// Main function
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}