#include "common_tests/test_utils.h"

// Main test runner for posit16 tests

// Main function
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Add the custom listener
  ::testing::TestEventListeners &listeners =
      ::testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new EpsilonStatisticsPrinter);

  return RUN_ALL_TESTS();
}