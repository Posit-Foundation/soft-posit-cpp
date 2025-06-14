#include <softposit_cpp.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

// Define the matrix dimensions for the example
const int SIZE       = 50;   // Size of the matrices
const int IMAGE_SIZE = 600;  // Output image size

// Function to initialize matrices with ill-conditioned values that highlight precision issues
template <typename T>
void initializeMatrices(std::vector<std::vector<T>> &A, std::vector<std::vector<T>> &B)
{
    // Use values that will cause cumulative rounding errors
    std::random_device rd;
    std::mt19937       gen(rd());

    // Create a distribution with very small and very large values
    std::uniform_real_distribution<> largeVals(0.1, 10.0);
    std::uniform_real_distribution<> smallVals(1e-8, 1e-5);
    std::uniform_real_distribution<> signs(0.0, 1.0);

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            // Mix large and small values to create ill-conditioned matrices
            double val1, val2;

            // Create pattern of large and small values
            if ((i + j) % 3 == 0)
            {
                val1 = largeVals(gen);
                val2 = smallVals(gen);
            }
            else if ((i + j) % 3 == 1)
            {
                val1 = smallVals(gen);
                val2 = largeVals(gen);
            }
            else
            {
                val1 = largeVals(gen) * 0.1;
                val2 = smallVals(gen) * 10.0;
            }

            // Randomly negate some values
            if (signs(gen) > 0.5)
                val1 = -val1;
            if (signs(gen) > 0.5)
                val2 = -val2;

            A[i][j] = T(val1);
            B[i][j] = T(val2);
        }
    }
}

// Matrix multiplication using float
void multiplyMatricesFloat(const std::vector<std::vector<float>> &A,
                           const std::vector<std::vector<float>> &B,
                           std::vector<std::vector<float>>       &C)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            C[i][j] = 0.0f;
            for (int k = 0; k < SIZE; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Matrix multiplication using posit32
void multiplyMatricesPosit(const std::vector<std::vector<posit32>> &A,
                           const std::vector<std::vector<posit32>> &B,
                           std::vector<std::vector<posit32>>       &C)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            C[i][j] = posit32(0.0);
            for (int k = 0; k < SIZE; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Same matrix multiplication using quire (accumulator) for high precision
void multiplyMatricesPositQuire(const std::vector<std::vector<posit32>> &A,
                                const std::vector<std::vector<posit32>> &B,
                                std::vector<std::vector<posit32>>       &C)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            // Initialize a quire32 and clear it
            quire32 q;
            q.clr();

            for (int k = 0; k < SIZE; k++)
            {
                // Use qma method to accumulate products with no intermediate rounding
                // qma means "quire multiply-add" -> q += a*b
                q.qma(A[i][k], B[k][j]);
            }

            // Convert back to posit
            C[i][j] = q.toPosit();
        }
    }
}

// Calculate mean absolute difference between matrices
template <typename T, typename U>
double calculateMeanDifference(const std::vector<std::vector<T>> &A,
                               const std::vector<std::vector<U>> &B)
{
    double sum = 0.0;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            // Handle type conversion properly
            double val1, val2;

            // Convert to double based on type
            if constexpr (std::is_same_v<T, posit32>)
            {
                val1 = convertP32ToDouble(castP32(A[i][j].value));
            }
            else
            {
                val1 = static_cast<double>(A[i][j]);
            }

            if constexpr (std::is_same_v<U, posit32>)
            {
                val2 = convertP32ToDouble(castP32(B[i][j].value));
            }
            else
            {
                val2 = static_cast<double>(B[i][j]);
            }

            sum += std::abs(val1 - val2);
        }
    }

    return sum / (SIZE * SIZE);
}

// Map matrix values to colors for visualization
void generateHeatmapImage(const std::vector<std::vector<double>> &diff, const std::string &filename)
{
    // Find min/max for color scaling
    double minVal = diff[0][0];
    double maxVal = diff[0][0];

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (diff[i][j] < minVal)
                minVal = diff[i][j];
            if (diff[i][j] > maxVal)
                maxVal = diff[i][j];
        }
    }

    // Create PPM image
    std::ofstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Could not create output file." << std::endl;
        return;
    }

    // Write PPM header (P6 format)
    file << "P6\n" << IMAGE_SIZE << " " << IMAGE_SIZE << "\n255\n";

    // Scale factor for mapping matrix to image
    double scale = static_cast<double>(IMAGE_SIZE) / SIZE;

    // Create the image data
    std::vector<unsigned char> image(IMAGE_SIZE * IMAGE_SIZE * 3);

    // Fill image with heatmap colors
    for (int y = 0; y < IMAGE_SIZE; y++)
    {
        for (int x = 0; x < IMAGE_SIZE; x++)
        {
            // Map image coordinates to matrix indices
            int i = static_cast<int>(y / scale);
            int j = static_cast<int>(x / scale);

            if (i >= SIZE)
                i = SIZE - 1;
            if (j >= SIZE)
                j = SIZE - 1;

            // Get normalized difference value
            double normalized = (diff[i][j] - minVal) / (maxVal - minVal);

            // Map to color (blue-white-red gradient)
            int r, g, b;
            if (normalized < 0.5)
            {
                // Blue to white
                double v = normalized * 2;
                r        = static_cast<int>(255 * v);
                g        = static_cast<int>(255 * v);
                b        = 255;
            }
            else
            {
                // White to red
                double v = (normalized - 0.5) * 2;
                r        = 255;
                g        = static_cast<int>(255 * (1.0 - v));
                b        = static_cast<int>(255 * (1.0 - v));
            }

            // Set pixel color
            int index        = (y * IMAGE_SIZE + x) * 3;
            image[index]     = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }

    // Write image data
    file.write(reinterpret_cast<const char *>(image.data()), image.size());

    std::cout << "Heatmap image saved to " << filename << std::endl;
}

// Generate a legend image to help interpret the heatmap
void generateLegendImage(const std::string &filename)
{
    const int width  = 600;
    const int height = 100;

    // Create PPM image
    std::ofstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Could not create legend file." << std::endl;
        return;
    }

    // Write PPM header
    file << "P6\n" << width << " " << height << "\n255\n";

    // Create image data
    std::vector<unsigned char> image(width * height * 3);

    // Fill with gradient
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double normalized = static_cast<double>(x) / width;

            // Map to color (blue-white-red gradient)
            int r, g, b;
            if (normalized < 0.5)
            {
                // Blue to white
                double v = normalized * 2;
                r        = static_cast<int>(255 * v);
                g        = static_cast<int>(255 * v);
                b        = 255;
            }
            else
            {
                // White to red
                double v = (normalized - 0.5) * 2;
                r        = 255;
                g        = static_cast<int>(255 * (1.0 - v));
                b        = static_cast<int>(255 * (1.0 - v));
            }

            // Set pixel color
            int index        = (y * width + x) * 3;
            image[index]     = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }

    // Write image data
    file.write(reinterpret_cast<const char *>(image.data()), image.size());

    std::cout << "Legend image saved to " << filename << std::endl;
}

int main()
{
    // Initialize matrices
    std::vector<std::vector<float>> A_float(SIZE, std::vector<float>(SIZE));
    std::vector<std::vector<float>> B_float(SIZE, std::vector<float>(SIZE));
    std::vector<std::vector<float>> C_float(SIZE, std::vector<float>(SIZE));

    std::vector<std::vector<posit32>> A_posit(SIZE, std::vector<posit32>(SIZE));
    std::vector<std::vector<posit32>> B_posit(SIZE, std::vector<posit32>(SIZE));
    std::vector<std::vector<posit32>> C_posit(SIZE, std::vector<posit32>(SIZE));
    std::vector<std::vector<posit32>> C_posit_quire(SIZE, std::vector<posit32>(SIZE));

    // Initialize with identical values
    initializeMatrices(A_float, B_float);

    // Copy to posit matrices
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            A_posit[i][j] = posit32(A_float[i][j]);
            B_posit[i][j] = posit32(B_float[i][j]);
        }
    }

    // Perform matrix multiplication
    std::cout << "Performing matrix multiplication with float..." << std::endl;
    multiplyMatricesFloat(A_float, B_float, C_float);

    std::cout << "Performing matrix multiplication with posit32..." << std::endl;
    multiplyMatricesPosit(A_posit, B_posit, C_posit);

    std::cout << "Performing matrix multiplication with posit32 and quire..." << std::endl;
    multiplyMatricesPositQuire(A_posit, B_posit, C_posit_quire);

    // Calculate differences for visualization
    std::vector<std::vector<double>> diff_float_posit(SIZE, std::vector<double>(SIZE));
    std::vector<std::vector<double>> diff_float_quire(SIZE, std::vector<double>(SIZE));
    std::vector<std::vector<double>> diff_posit_quire(SIZE, std::vector<double>(SIZE));

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            double float_val = static_cast<double>(C_float[i][j]);
            double posit_val = convertP32ToDouble(castP32(C_posit[i][j].value));
            double quire_val = convertP32ToDouble(castP32(C_posit_quire[i][j].value));

            // Calculate absolute differences
            diff_float_posit[i][j] = std::abs(float_val - posit_val);
            diff_float_quire[i][j] = std::abs(float_val - quire_val);
            diff_posit_quire[i][j] = std::abs(posit_val - quire_val);
        }
    }

    // Calculate mean absolute differences
    double mean_diff_float_posit = calculateMeanDifference(C_float, C_posit);
    double mean_diff_float_quire = calculateMeanDifference(C_float, C_posit_quire);
    double mean_diff_posit_quire = calculateMeanDifference(C_posit, C_posit_quire);

    std::cout << "\nPrecision Comparison Results:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Mean absolute difference between float and posit32: " << mean_diff_float_posit
              << std::endl;
    std::cout << "Mean absolute difference between float and posit32+quire: "
              << mean_diff_float_quire << std::endl;
    std::cout << "Mean absolute difference between posit32 and posit32+quire: "
              << mean_diff_posit_quire << std::endl;

    // Generate heatmap images
    std::cout << "\nGenerating heatmap images..." << std::endl;
    generateHeatmapImage(diff_float_posit, "matrix_diff_float_posit.ppm");
    generateHeatmapImage(diff_float_quire, "matrix_diff_float_quire.ppm");
    generateHeatmapImage(diff_posit_quire, "matrix_diff_posit_quire.ppm");
    generateLegendImage("matrix_heatmap_legend.ppm");

    std::cout << "\nMatrix multiplication precision comparison complete." << std::endl;
    std::cout << "The heatmap images show where precision differences occur:" << std::endl;
    std::cout << "  - Blue areas indicate small differences (high agreement)" << std::endl;
    std::cout << "  - Red areas indicate large differences (precision issues)" << std::endl;
    std::cout << "  - View the PPM files with any image viewer that supports the format"
              << std::endl;

    return 0;
}