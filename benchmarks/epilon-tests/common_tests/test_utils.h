#ifndef SOFTPOSIT_COMMON_TEST_UTILS_H
#define SOFTPOSIT_COMMON_TEST_UTILS_H

#include "softposit_cpp.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <vector>

// Define constants for number of tests
#ifdef NDEBUG
constexpr int NTESTS8 = 10000000;
constexpr int NTESTS16 = 10000000;
constexpr int NTESTS32 = 10000000;
#else
constexpr int NTESTS8 = 100000;
constexpr int NTESTS16 = 1000000;
constexpr int NTESTS32 = 1000000;
#endif

// Random generator setup - global to be used across all test files
extern std::mt19937 gen;

// Random distributions for p8 tests
extern std::uniform_int_distribution<int8_t> int_dist8;

// Random distributions for p16 tests
extern std::uniform_int_distribution<int16_t> int_dist16;

// Random distributions for p32 tests
extern std::uniform_int_distribution<int32_t> int_dist32;

// Current operation being tested
extern std::string current_operation;

// Global vector to store all epsilon values
extern std::vector<double> all_epsilons;

// Map to store epsilon values for each operation
extern std::map<std::string, std::vector<double>> operation_epsilons;

// Function to calculate the smallest epsilon that makes a and b equal
double find_smallest_epsilon(double a, double b, double epsilon_min = 1e-12,
                             double epsilon_max = 1e-5);

// Compare doubles with epsilon tolerance
bool double_eq(double a, double b, double epsilon_min = 1e-12,
               double epsilon_max = 1e-5);

// Function to calculate and print statistics for a vector of epsilon values
void printEpsilonStats(const std::string &label,
                       const std::vector<double> &epsilons);

// Class for epsilon statistics
class EpsilonStatisticsPrinter : public ::testing::EmptyTestEventListener {
public:
  virtual void OnTestProgramEnd(const ::testing::UnitTest &unit_test);
};

// Templated generic exact test function for posit operations
template <typename PositType>
void test_exact(int num_tests,
                std::function<PositType(PositType, PositType)> posit_op,
                std::function<double(double, double)> double_op,
                std::function<int(void)> random_generator) {
  for (int i = 0; i < num_tests; i++) {
    // Generate random posit values
    auto raw_a = random_generator();
    auto raw_b = random_generator();

    // Create posit values
    PositType p_a, p_b;
    p_a.value = raw_a;
    p_b.value = raw_b;

    // Convert to double
    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();

    // Handle NaR (Not a Real) cases in posit
    if (p_a.isNaR() || p_b.isNaR() || std::isnan(f_a) || std::isnan(f_b)) {
      // If either input is NaR/NaN, skip this test case
      continue;
    }

    // Calculate results
    PositType result = posit_op(p_a, p_b);
    double expected_double = double_op(f_a, f_b);
    PositType expected = expected_double;

    // Compare results
    ASSERT_TRUE(double_eq(result.toDouble(), expected.toDouble()))
        << "Input: (" << f_a << ", " << f_b << ")"
        << "\nResult: " << result.toDouble()
        << "\nExpected: " << expected_double;
  }
}

// Templated ULP function (Unit in Last Place)
template <typename PositType>
auto ulp(PositType x, PositType y) -> decltype(x.value) {
  // Cast to the appropriate type to avoid ambiguity with std::abs
  using ValueType = decltype(x.value);
  return static_cast<ValueType>(std::abs(static_cast<int64_t>(x.value) - static_cast<int64_t>(y.value)));
}

#endif // SOFTPOSIT_COMMON_TEST_UTILS_H