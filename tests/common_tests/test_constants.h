// tests/common_tests/test_constants.h
#ifndef SOFTPOSIT_TEST_CONSTANTS_H
#define SOFTPOSIT_TEST_CONSTANTS_H

namespace softposit_test
{

// Define constants for number of tests
#ifdef NDEBUG
    constexpr int NTESTS8 = 10000;
    constexpr int NTESTS16 = 10000;
    constexpr int NTESTS32 = 5000;
#else
    constexpr int NTESTS8 = 1000;
    constexpr int NTESTS16 = 1000;
    constexpr int NTESTS32 = 500;
#endif

} // namespace softposit_test

#endif // SOFTPOSIT_TEST_CONSTANTS_H