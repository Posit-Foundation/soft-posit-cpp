#include "common_tests/test_utils.h"

// Main test runner for posit16 tests

// Main function
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}