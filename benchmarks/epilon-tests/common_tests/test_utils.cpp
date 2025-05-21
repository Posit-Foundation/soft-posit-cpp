#include "test_utils.h"

// Initialize global random generator
std::random_device rd;
std::mt19937 gen(rd());

// Random distributions for p8 tests
std::uniform_int_distribution<int8_t> int_dist8(INT8_MIN, INT8_MAX);

// Random distributions for p16 tests
std::uniform_int_distribution<int16_t> int_dist16(INT16_MIN, INT16_MAX);

// Random distributions for p32 tests
std::uniform_int_distribution<int32_t> int_dist32(INT32_MIN, INT32_MAX);

// Initialize global operation tracking
std::string current_operation = "Unknown";

// Initialize global epsilon tracking
std::vector<double> all_epsilons;
std::map<std::string, std::vector<double>> operation_epsilons;

// Function to calculate the smallest epsilon that makes a and b equal
double find_smallest_epsilon(double a, double b, double epsilon_min,
                             double epsilon_max) {
  // Step size calculated based on range and desired iterations
  int num_steps = 1000;
  double step = (epsilon_max - epsilon_min) / num_steps;

  for (double epsilon = epsilon_min; epsilon <= epsilon_max; epsilon += step) {
    // Absolute difference check
    if (std::abs(a - b) < epsilon) {
      return epsilon;
    }

    // Relative difference check (with small denominator guard)
    double rel_diff = std::abs(a - b) / (std::abs(a) + std::abs(b) + 1e-10);
    if (rel_diff < epsilon) {
      return epsilon;
    }
  }

  return epsilon_max +
         step; // Return a value larger than epsilon_max if no match
}

// Compare doubles with epsilon tolerance and track for statistics
bool double_eq(double a, double b, double epsilon_min, double epsilon_max) {
  // Special case: If both values are NaN, consider them equal
  if (std::isnan(a) && std::isnan(b)) {
    return true;
  }

  // If only one value is NaN, they are not equal
  if (std::isnan(a) || std::isnan(b)) {
    return false;
  }

  // Calculate the smallest epsilon that makes a and b equal
  double smallest_epsilon =
      find_smallest_epsilon(a, b, epsilon_min, epsilon_max);

  // Store the epsilon value for later analysis
  if (smallest_epsilon <= epsilon_max) {
    all_epsilons.push_back(smallest_epsilon);

    // Store for per-operation analysis
    if (!current_operation.empty()) {
      operation_epsilons[current_operation].push_back(smallest_epsilon);
    }

    return true;
  }

  return false;
}

// Function to calculate and print statistics for a vector of epsilon values
void printEpsilonStats(const std::string &label,
                       const std::vector<double> &epsilons) {
  if (epsilons.empty()) {
    std::cout << label << ": No epsilon values were recorded." << std::endl;
    return;
  }

  // Calculate min
  double min_epsilon = *std::min_element(epsilons.begin(), epsilons.end());

  // Calculate max
  double max_epsilon = *std::max_element(epsilons.begin(), epsilons.end());

  // Calculate mean
  double sum = std::accumulate(epsilons.begin(), epsilons.end(), 0.0);
  double mean = sum / epsilons.size();

  // Calculate standard deviation
  double sq_sum = std::inner_product(epsilons.begin(), epsilons.end(),
                                     epsilons.begin(), 0.0, std::plus<double>(),
                                     [](double a, double b) { return a * b; });
  double std_dev = std::sqrt(sq_sum / epsilons.size() - mean * mean);

  // Calculate median
  std::vector<double> sorted_epsilons = epsilons;
  std::sort(sorted_epsilons.begin(), sorted_epsilons.end());
  double median;
  size_t size = sorted_epsilons.size();
  if (size % 2 == 0) {
    median = (sorted_epsilons[size / 2 - 1] + sorted_epsilons[size / 2]) / 2.0;
  } else {
    median = sorted_epsilons[size / 2];
  }

  // Calculate percentiles (25th and 75th)
  double p25 = sorted_epsilons[size / 4];
  double p75 = sorted_epsilons[3 * size / 4];

  // Print statistics
  std::cout << "\n===== " << label << " =====" << std::endl;
  std::cout << "Total comparisons: " << epsilons.size() << std::endl;
  std::cout << "Minimum epsilon: " << min_epsilon << std::endl;
  std::cout << "Maximum epsilon: " << max_epsilon << std::endl;
  std::cout << "Mean epsilon: " << mean << std::endl;
  std::cout << "Median epsilon: " << median << std::endl;
  std::cout << "25th percentile: " << p25 << std::endl;
  std::cout << "75th percentile: " << p75 << std::endl;
  std::cout << "Standard deviation: " << std_dev << std::endl;

  // Histogram functionality is commented out but kept in case needed in the
  // future
  /*
  const int num_bins = 10;
  std::vector<int> histogram(num_bins, 0);
  double bin_width = (max_epsilon - min_epsilon) / num_bins;
  if (bin_width > 0) { // Prevent division by zero
      for (double epsilon : epsilons) {
          int bin = std::min(static_cast<int>((epsilon - min_epsilon) /
  bin_width), num_bins - 1); histogram[bin]++;
      }

      std::cout << "\nHistogram of epsilon values:" << std::endl;
      for (int i = 0; i < num_bins; i++) {
          double bin_start = min_epsilon + i * bin_width;
          double bin_end = bin_start + bin_width;
          std::cout << "[" << bin_start << " - " << bin_end << "): "
                    << histogram[i] << " ("
                    << (100.0 * histogram[i] / epsilons.size()) << "%)" <<
  std::endl;
      }
  } else {
      std::cout << "\nAll epsilon values are identical: " << min_epsilon <<
  std::endl;
  }
  */
}

// Implementation of the test statistics printer
void EpsilonStatisticsPrinter::OnTestProgramEnd(
    const ::testing::UnitTest &unit_test) {
  // First print global statistics
  printEpsilonStats("Global Epsilon Statistics", all_epsilons);

  // Then print per-operation statistics
  for (const auto &pair : operation_epsilons) {
    printEpsilonStats(pair.first + " Operation Statistics", pair.second);
  }

  // Print operation comparison
  if (!operation_epsilons.empty()) {
    std::cout << "\n===== Operation Comparison =====" << std::endl;
    std::cout << "Operation\tCount\tMin\tMedian\tMean\tMax\tStd Dev"
              << std::endl;

    for (const auto &pair : operation_epsilons) {
      const std::string &op = pair.first;
      const std::vector<double> &epsilons = pair.second;

      if (epsilons.empty())
        continue;

      // Calculate statistics
      double min = *std::min_element(epsilons.begin(), epsilons.end());
      double max = *std::max_element(epsilons.begin(), epsilons.end());
      double sum = std::accumulate(epsilons.begin(), epsilons.end(), 0.0);
      double mean = sum / epsilons.size();

      // Calculate median
      std::vector<double> sorted = epsilons;
      std::sort(sorted.begin(), sorted.end());
      double median;
      if (sorted.size() % 2 == 0) {
        median =
            (sorted[sorted.size() / 2 - 1] + sorted[sorted.size() / 2]) / 2.0;
      } else {
        median = sorted[sorted.size() / 2];
      }

      // Calculate standard deviation
      double sq_sum = std::inner_product(
          epsilons.begin(), epsilons.end(), epsilons.begin(), 0.0,
          std::plus<double>(), [](double a, double b) { return a * b; });
      double std_dev = std::sqrt(sq_sum / epsilons.size() - mean * mean);

      // Print in tabular format
      std::cout << op << "\t" << epsilons.size() << "\t" << min << "\t"
                << median << "\t" << mean << "\t" << max << "\t" << std_dev
                << std::endl;
    }
  }
}