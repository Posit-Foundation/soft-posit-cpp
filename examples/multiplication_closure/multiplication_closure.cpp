#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "softposit.h"

// Constants for image generation
const int    IMAGE_SIZE = 1000;
const double RANGE      = 1e10;  // Range of values to test (-RANGE to +RANGE)

// Enum for result types
enum ResultType
{
    RESULT_EXACT     = 0,
    RESULT_INEXACT   = 1,
    RESULT_OVERFLOW  = 2,
    RESULT_UNDERFLOW = 3,
    RESULT_NAN       = 4  // Fixed name to avoid conflicts
};

// Structure to hold statistics
struct MultiplicationStats
{
    unsigned long long total;
    unsigned long long exact;
    unsigned long long inexact;
    unsigned long long nan;
    unsigned long long overflow;
    unsigned long long underflow;

    MultiplicationStats() : total(0), exact(0), inexact(0), nan(0), overflow(0), underflow(0)
    {
    }

    void printPercentages() const
    {
        std::cout << "\nMultiplication Closure Statistics:\n";
        std::cout << "================================\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Total operations: " << total << "\n";
        std::cout << "Exact:      " << (100.0 * exact / total) << "%\n";
        std::cout << "Inexact:    " << (100.0 * inexact / total) << "%\n";
        std::cout << "NaN:        " << (100.0 * nan / total) << "%\n";
        std::cout << "Overflow:   " << (100.0 * overflow / total) << "%\n";
        std::cout << "Underflow:  " << (100.0 * underflow / total) << "%\n";
    }
};

// Function to check if a posit is NaN
bool isNaN(posit16_t p)
{
    // Check the bit pattern for NaN in posit
    return p.v == 0x8000;  // 0x8000 is the standard NaN encoding for posit16
}

// Function to check if a multiplication result is exact
bool isExact(posit16_t a, posit16_t b, posit16_t result)
{
    // Convert to double for comparison
    double da      = convertP16ToDouble(a);
    double db      = convertP16ToDouble(b);
    double dresult = convertP16ToDouble(result);

    // Check if the result matches the double precision multiplication
    return std::abs((da * db) - dresult) < 1e-10;
}

// Function to get the color for a specific result type
void getColor(ResultType type, unsigned char& r, unsigned char& g, unsigned char& b)
{
    switch (type)
    {
        case RESULT_EXACT:
            r = 0;
            g = 0;
            b = 0;  // Black
            break;
        case RESULT_INEXACT:
            r = 128;
            g = 0;
            b = 128;  // Purple
            break;
        case RESULT_OVERFLOW:
            r = 255;
            g = 0;
            b = 0;  // Red
            break;
        case RESULT_UNDERFLOW:
            r = 0;
            g = 0;
            b = 255;  // Blue
            break;
        case RESULT_NAN:
            r = 255;
            g = 255;
            b = 0;  // Yellow
            break;
        default:
            r = 128;
            g = 128;
            b = 128;  // Grey
    }
}

// Function to test multiplication and categorize results
ResultType testMultiplication(posit16_t a, posit16_t b, MultiplicationStats& stats)
{
    stats.total++;

    posit16_t result = p16_mul(a, b);

    if (isNaN(result))
    {
        stats.nan++;
        return RESULT_NAN;
    }
    else if (isExact(a, b, result))
    {
        stats.exact++;
        return RESULT_EXACT;
    }
    else
    {
        stats.inexact++;
        return RESULT_INEXACT;
    }

    // Posits don't have traditional overflow/underflow, but we keep these categories
    // for compatibility with the image format
}

// Create PPM image of the multiplication results
void saveMultiplicationImage(const std::vector<std::vector<ResultType>>& resultMatrix,
                             const char*                                 filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write PPM header
    file << "P6\n" << IMAGE_SIZE << " " << IMAGE_SIZE << "\n255\n";

    // Write pixel data
    for (int y = 0; y < IMAGE_SIZE; y++)
    {
        for (int x = 0; x < IMAGE_SIZE; x++)
        {
            unsigned char r, g, b;
            getColor(resultMatrix[y][x], r, g, b);
            file.write(reinterpret_cast<const char*>(&r), 1);
            file.write(reinterpret_cast<const char*>(&g), 1);
            file.write(reinterpret_cast<const char*>(&b), 1);
        }
    }

    file.close();
    std::cout << "Image saved to " << filename << std::endl;
}

int main()
{
    MultiplicationStats stats;

    // Create a matrix to store results for visualization
    std::vector<std::vector<ResultType>> resultMatrix(IMAGE_SIZE,
                                                      std::vector<ResultType>(IMAGE_SIZE));

    // Test special cases that produce NaN
    std::cout << "Testing special NaN cases:\n";

    // Test ±∞ × 0
    posit16_t zero    = convertDoubleToP16(0.0);
    posit16_t inf     = p16_div(convertDoubleToP16(1.0), zero);  // Creates infinity
    posit16_t neg_inf = p16_div(convertDoubleToP16(-1.0), zero);

    std::cout << "inf * 0 = " << (isNaN(p16_mul(inf, zero)) ? "NaN" : "not NaN") << "\n";
    std::cout << "-inf * 0 = " << (isNaN(p16_mul(neg_inf, zero)) ? "NaN" : "not NaN") << "\n";

    // Test regular cases
    std::cout << "\nGenerating multiplication closure plot...\n";

    // Map each pixel to a value in our range
    for (int y = 0; y < IMAGE_SIZE; y++)
    {
        for (int x = 0; x < IMAGE_SIZE; x++)
        {
            // Map pixel coordinates to values in our range
            double a_val = (2.0 * x / IMAGE_SIZE - 1.0) * RANGE;
            double b_val = (2.0 * y / IMAGE_SIZE - 1.0) * RANGE;

            posit16_t a = convertDoubleToP16(a_val);
            posit16_t b = convertDoubleToP16(b_val);

            // Test multiplication and store result type
            resultMatrix[y][x] = testMultiplication(a, b, stats);
        }

        // Show progress
        if (y % 80 == 0)
        {
            std::cout << "Progress: " << (100.0 * y / IMAGE_SIZE) << "%" << std::endl;
        }
    }

    // Print statistics
    stats.printPercentages();

    // Save the visualization
    saveMultiplicationImage(resultMatrix, "multiplication_closure.ppm");

    return 0;
}