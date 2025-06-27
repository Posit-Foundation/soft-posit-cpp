#include <softposit.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Constants for image dimensions and FFT
const int    IMAGE_SIZE = 256;  // Use a power of 2 for efficient FFT
const double PI         = 3.14159265358979323846;

// Define RGB color struct for image representation
struct RGB
{
    unsigned char r, g, b;
    RGB(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) : r(r), g(g), b(b)
    {
    }
};

// Forward declarations for overloaded operators and conversion functions for posit
inline posit32_t operator+(const posit32_t& a, const posit32_t& b)
{
    return p32_add(a, b);
}

inline posit32_t operator-(const posit32_t& a, const posit32_t& b)
{
    return p32_sub(a, b);
}

inline posit32_t operator*(const posit32_t& a, const posit32_t& b)
{
    return p32_mul(a, b);
}

inline posit32_t operator/(const posit32_t& a, const posit32_t& b)
{
    return p32_div(a, b);
}

inline posit32_t ui32_to_p32(uint32_t a)
{
    posit32_t p;
    p.v = a;
    return p;
}

inline posit32_t int_to_posit32(int value)
{
    return ui32_to_p32(value);
}

inline posit32_t double_to_posit32(double value)
{
    return convertDoubleToP32(value);
}

inline double posit32_to_double(posit32_t value)
{
    return convertP32ToDouble(value);
}

// Complex number template for both float and posit32_t
template <typename T>
class Complex
{
   public:
    T real;
    T imag;

    // Default constructor
    Complex()
    {
        if constexpr (std::is_same_v<T, posit32_t>)
        {
            real = double_to_posit32(0.0);
            imag = double_to_posit32(0.0);
        }
        else
        {
            real = T(0);
            imag = T(0);
        }
    }

    // Constructor with T type parameters
    Complex(T r, T i) : real(r), imag(i)
    {
    }

    // Cast constructor for posit32_t to handle initialization with numeric literals
    template <
        typename P,
        typename std::enable_if_t<std::is_same_v<T, posit32_t> && std::is_arithmetic_v<P>, int> = 0>
    Complex(P r, P i)
    {
        real = double_to_posit32(static_cast<double>(r));
        imag = double_to_posit32(static_cast<double>(i));
    }

    // Cast constructor for float to handle initialization with numeric literals
    template <typename F,
              typename std::enable_if_t<std::is_same_v<T, float> && std::is_arithmetic_v<F> &&
                                            !std::is_same_v<F, posit32_t>,
                                        int> = 0>
    Complex(F r, F i)
    {
        real = static_cast<float>(r);
        imag = static_cast<float>(i);
    }

    // Copy assignment operator
    Complex<T>& operator=(const Complex<T>& other)
    {
        real = other.real;
        imag = other.imag;
        return *this;
    }

    // Addition
    Complex<T> operator+(const Complex<T>& other) const
    {
        return Complex<T>(real + other.real, imag + other.imag);
    }

    // Subtraction
    Complex<T> operator-(const Complex<T>& other) const
    {
        return Complex<T>(real - other.real, imag - other.imag);
    }

    // Multiplication
    Complex<T> operator*(const Complex<T>& other) const
    {
        T a = real;
        T b = imag;
        T c = other.real;
        T d = other.imag;
        return Complex<T>(a * c - b * d, a * d + b * c);
    }

    // Division
    Complex<T> operator/(const Complex<T>& other) const
    {
        T a     = real;
        T b     = imag;
        T c     = other.real;
        T d     = other.imag;
        T denom = c * c + d * d;
        return Complex<T>((a * c + b * d) / denom, (b * c - a * d) / denom);
    }

    // Scaling by a scalar
    Complex<T> operator*(T scalar) const
    {
        return Complex<T>(real * scalar, imag * scalar);
    }

    // Magnitude squared
    T magnitudeSquared() const
    {
        return real * real + imag * imag;
    }

    // Convert to double (for posit types)
    std::complex<double> toDouble() const
    {
        if constexpr (std::is_same_v<T, posit32_t>)
        {
            return std::complex<double>(posit32_to_double(real), posit32_to_double(imag));
        }
        else
        {
            return std::complex<double>(real, imag);
        }
    }
};

// Specializations for quire accumulation with posit32_t
template <>
Complex<posit32_t> Complex<posit32_t>::operator*(const Complex<posit32_t>& other) const
{
    posit32_t a = real;
    posit32_t b = imag;
    posit32_t c = other.real;
    posit32_t d = other.imag;

    // Standard multiplication
    return Complex<posit32_t>(a * c - b * d, a * d + b * c);
}

// Function to perform multiplication with quire accumulation
inline Complex<posit32_t> multiplyComplexWithQuire(const Complex<posit32_t>& x,
                                                   const Complex<posit32_t>& y)
{
    posit32_t a = x.real;
    posit32_t b = x.imag;
    posit32_t c = y.real;
    posit32_t d = y.imag;

    // Real part: a*c - b*d
    quire32_t qReal;
    qReal = q32_clr(qReal);
    qReal = q32_fdp_add(qReal, a, c);  // a*c
    qReal = q32_fdp_sub(qReal, b, d);  // a*c - b*d

    // Imaginary part: a*d + b*c
    quire32_t qImag;
    qImag = q32_clr(qImag);
    qImag = q32_fdp_add(qImag, a, d);  // a*d
    qImag = q32_fdp_add(qImag, b, c);  // a*d + b*c

    return Complex<posit32_t>(q32_to_p32(qReal), q32_to_p32(qImag));
}

// Save PPM image function
void savePPM(const std::string& filename, const std::vector<RGB>& pixels, int width, int height)
{
    std::ofstream outfile(filename, std::ios::binary);
    outfile << "P6\n" << width << " " << height << "\n255\n";

    for (const auto& pixel : pixels)
    {
        outfile.write(reinterpret_cast<const char*>(&pixel), 3);
    }

    outfile.close();
    std::cout << "Saved image to " << filename << std::endl;
}

// Generate test image with features that highlight precision issues
std::vector<RGB> generateTestImage(int width, int height)
{
    std::vector<RGB> image(width * height);

    // Create a pattern that includes:
    // 1. Sharp edges (to test high frequency components)
    // 2. Smooth gradients (to test precision in reconstruction)
    // 3. Fine details (to test information preservation)

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Calculate normalized coordinates
            double nx = static_cast<double>(x) / width;
            double ny = static_cast<double>(y) / height;

            // Base color - smooth gradient
            double r = std::sin(nx * PI) * 128 + 127;
            double g = std::sin(ny * PI) * 128 + 127;
            double b = std::sin((nx + ny) * PI) * 128 + 127;

            // Add checkerboard pattern (sharp edges)
            if ((x / 32 + y / 32) % 2 == 0)
            {
                r = 255 - r;
                g = 255 - g;
            }

            // Add fine detail circles
            double cx   = nx - 0.5;
            double cy   = ny - 0.5;
            double dist = std::sqrt(cx * cx + cy * cy);

            // Concentric circles
            if (std::abs(std::sin(dist * 50) * std::cos(dist * 50)) > 0.9)
            {
                b = 255;
                r = 50;
            }

            // Add some high-frequency components
            if ((x % 4 == 0) && (y % 4 == 0))
            {
                r = 255;
                g = 255;
                b = 255;
            }

            image[y * width + x] = RGB(static_cast<unsigned char>(r),
                                       static_cast<unsigned char>(g),
                                       static_cast<unsigned char>(b));
        }
    }

    return image;
}

// Convert RGB image to grayscale for FFT processing
template <typename T>
std::vector<T> convertToGrayscale(const std::vector<RGB>& image)
{
    std::vector<T> result(image.size());

    for (size_t i = 0; i < image.size(); ++i)
    {
        // Standard grayscale conversion: 0.299*R + 0.587*G + 0.114*B
        double gray = 0.299 * image[i].r + 0.587 * image[i].g + 0.114 * image[i].b;

        if constexpr (std::is_same_v<T, posit32_t>)
        {
            result[i] = double_to_posit32(gray);
        }
        else
        {
            result[i] = static_cast<T>(gray);
        }
    }

    return result;
}

// Convert grayscale FFT result back to RGB image
std::vector<RGB> convertToRGB(const std::vector<float>& grayscale)
{
    std::vector<RGB> result(grayscale.size());

    for (size_t i = 0; i < grayscale.size(); ++i)
    {
        unsigned char value = static_cast<unsigned char>(std::clamp(grayscale[i], 0.0f, 255.0f));
        result[i]           = RGB(value, value, value);
    }

    return result;
}

// Convert grayscale FFT result back to RGB image for posit
std::vector<RGB> convertToRGB(const std::vector<posit32_t>& grayscale)
{
    std::vector<RGB> result(grayscale.size());

    for (size_t i = 0; i < grayscale.size(); ++i)
    {
        double        value      = posit32_to_double(grayscale[i]);
        unsigned char pixelValue = static_cast<unsigned char>(std::clamp(value, 0.0, 255.0));
        result[i]                = RGB(pixelValue, pixelValue, pixelValue);
    }

    return result;
}

// Create a visualization of the error between original and reconstructed images
std::vector<RGB> visualizeError(const std::vector<RGB>& original,
                                const std::vector<RGB>& reconstructed)
{
    std::vector<RGB> errorImage(original.size());

    for (size_t i = 0; i < original.size(); ++i)
    {
        // Calculate absolute differences for each channel
        int rDiff =
            std::abs(static_cast<int>(original[i].r) - static_cast<int>(reconstructed[i].r));
        int gDiff =
            std::abs(static_cast<int>(original[i].g) - static_cast<int>(reconstructed[i].g));
        int bDiff =
            std::abs(static_cast<int>(original[i].b) - static_cast<int>(reconstructed[i].b));

        // Scale up the errors to make them more visible (multiply by 5)
        rDiff = std::min(255, rDiff * 5);
        gDiff = std::min(255, gDiff * 5);
        bDiff = std::min(255, bDiff * 5);

        errorImage[i] = RGB(static_cast<unsigned char>(rDiff),
                            static_cast<unsigned char>(gDiff),
                            static_cast<unsigned char>(bDiff));
    }

    return errorImage;
}

// Convert spatial domain data to frequency domain using FFT (radix-2 algorithm)
template <typename T>
std::vector<Complex<T>> fft(const std::vector<Complex<T>>& spatial)
{
    int n = spatial.size();

    // Base case for recursion
    if (n <= 1)
    {
        return spatial;
    }

    // Split into even and odd indices
    std::vector<Complex<T>> even(n / 2);
    std::vector<Complex<T>> odd(n / 2);

    for (int i = 0; i < n / 2; ++i)
    {
        even[i] = spatial[2 * i];
        odd[i]  = spatial[2 * i + 1];
    }

    // Recursive calls
    std::vector<Complex<T>> evenFFT = fft(even);
    std::vector<Complex<T>> oddFFT  = fft(odd);

    // Combine results
    std::vector<Complex<T>> result(n);

    for (int k = 0; k < n / 2; ++k)
    {
        // Calculate twiddle factor: e^(-2πi*k/n)
        double     angle = -2.0 * PI * k / n;
        Complex<T> twiddle;

        if constexpr (std::is_same_v<T, posit32_t>)
        {
            twiddle =
                Complex<T>(double_to_posit32(std::cos(angle)), double_to_posit32(std::sin(angle)));
        }
        else
        {
            twiddle = Complex<T>(static_cast<T>(std::cos(angle)), static_cast<T>(std::sin(angle)));
        }

        // Standard butterfly operation
        result[k]         = evenFFT[k] + twiddle * oddFFT[k];
        result[k + n / 2] = evenFFT[k] - twiddle * oddFFT[k];
    }

    return result;
}

// Special version of FFT using quire for complex multiplication
std::vector<Complex<posit32_t>> fftWithQuire(const std::vector<Complex<posit32_t>>& spatial)
{
    int n = spatial.size();

    // Base case for recursion
    if (n <= 1)
    {
        return spatial;
    }

    // Split into even and odd indices
    std::vector<Complex<posit32_t>> even(n / 2);
    std::vector<Complex<posit32_t>> odd(n / 2);

    for (int i = 0; i < n / 2; ++i)
    {
        even[i] = spatial[2 * i];
        odd[i]  = spatial[2 * i + 1];
    }

    // Recursive calls
    std::vector<Complex<posit32_t>> evenFFT = fftWithQuire(even);
    std::vector<Complex<posit32_t>> oddFFT  = fftWithQuire(odd);

    // Combine results
    std::vector<Complex<posit32_t>> result(n);

    for (int k = 0; k < n / 2; ++k)
    {
        // Calculate twiddle factor: e^(-2πi*k/n)
        double             angle = -2.0 * PI * k / n;
        Complex<posit32_t> twiddle(double_to_posit32(std::cos(angle)),
                                   double_to_posit32(std::sin(angle)));

        // Butterfly operation with quire accumulation
        Complex<posit32_t> twiddledOdd = multiplyComplexWithQuire(twiddle, oddFFT[k]);
        result[k]                      = evenFFT[k] + twiddledOdd;
        result[k + n / 2]              = evenFFT[k] - twiddledOdd;
    }

    return result;
}

// Inverse FFT
template <typename T>
std::vector<Complex<T>> ifft(const std::vector<Complex<T>>& frequency)
{
    int n = frequency.size();

    // Conjugate the input
    std::vector<Complex<T>> conjugated(n);
    for (int i = 0; i < n; ++i)
    {
        if constexpr (std::is_same_v<T, posit32_t>)
        {
            conjugated[i] = Complex<T>(frequency[i].real, double_to_posit32(0) - frequency[i].imag);
        }
        else
        {
            conjugated[i] = Complex<T>(frequency[i].real, -frequency[i].imag);
        }
    }

    // Perform forward FFT on conjugated input
    std::vector<Complex<T>> transformed = fft(conjugated);

    // Conjugate the output and scale by 1/n
    std::vector<Complex<T>> result(n);
    for (int i = 0; i < n; ++i)
    {
        if constexpr (std::is_same_v<T, posit32_t>)
        {
            T scaling = double_to_posit32(1.0 / n);
            result[i] = Complex<T>(transformed[i].real * scaling,
                                   (double_to_posit32(0) - transformed[i].imag) * scaling);
        }
        else
        {
            T scaling = static_cast<T>(1.0 / n);
            result[i] = Complex<T>(transformed[i].real * scaling, -transformed[i].imag * scaling);
        }
    }

    return result;
}

// Special version of IFFT using quire for complex multiplication
std::vector<Complex<posit32_t>> ifftWithQuire(const std::vector<Complex<posit32_t>>& frequency)
{
    int n = frequency.size();

    // Conjugate the input
    std::vector<Complex<posit32_t>> conjugated(n);
    for (int i = 0; i < n; ++i)
    {
        conjugated[i] =
            Complex<posit32_t>(frequency[i].real, double_to_posit32(0) - frequency[i].imag);
    }

    // Perform forward FFT on conjugated input using quire
    std::vector<Complex<posit32_t>> transformed = fftWithQuire(conjugated);

    // Conjugate the output and scale by 1/n
    std::vector<Complex<posit32_t>> result(n);
    posit32_t                       scaling = double_to_posit32(1.0 / n);

    for (int i = 0; i < n; ++i)
    {
        // Scale with quire accumulation
        quire32_t qReal;
        qReal = q32_clr(qReal);
        qReal = q32_fdp_add(qReal, transformed[i].real, scaling);

        quire32_t qImag;
        qImag = q32_clr(qImag);
        qImag = q32_fdp_add(qImag, double_to_posit32(0) - transformed[i].imag, scaling);

        result[i] = Complex<posit32_t>(q32_to_p32(qReal), q32_to_p32(qImag));
    }

    return result;
}

// Apply a filter in the frequency domain
template <typename T>
void applyFilter(std::vector<Complex<T>>& frequency, int size)
{
    // Simple low-pass filter that removes high-frequency components
    double centerX      = size / 2;
    double centerY      = size / 2;
    double cutoffRadius = size / 4;  // Keep only the lowest 25% frequencies

    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            double distFromCenter =
                std::sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));

            if (distFromCenter > cutoffRadius)
            {
                // Set high frequencies to zero
                int index = y * size + x;

                if constexpr (std::is_same_v<T, posit32_t>)
                {
                    frequency[index] = Complex<T>(double_to_posit32(0), double_to_posit32(0));
                }
                else
                {
                    frequency[index] = Complex<T>(0, 0);
                }
            }
        }
    }
}

// Calculate the PSNR (Peak Signal-to-Noise Ratio) between original and reconstructed images
double calculatePSNR(const std::vector<RGB>& original, const std::vector<RGB>& reconstructed)
{
    double mse = 0.0;

    for (size_t i = 0; i < original.size(); ++i)
    {
        double rDiff = static_cast<double>(original[i].r) - static_cast<double>(reconstructed[i].r);
        double gDiff = static_cast<double>(original[i].g) - static_cast<double>(reconstructed[i].g);
        double bDiff = static_cast<double>(original[i].b) - static_cast<double>(reconstructed[i].b);

        mse += (rDiff * rDiff + gDiff * gDiff + bDiff * bDiff) / 3.0;
    }

    mse /= original.size();

    if (mse <= 1e-10)
    {
        return 100.0;  // Return a high value for perfect reconstruction
    }

    double maxIntensity = 255.0;
    double psnr         = 10.0 * std::log10((maxIntensity * maxIntensity) / mse);

    return psnr;
}

// Create a side-by-side comparison image
std::vector<RGB> createComparisonImage(const std::vector<RGB>& original,
                                       const std::vector<RGB>& floatResult,
                                       const std::vector<RGB>& positResult,
                                       const std::vector<RGB>& quireResult,
                                       int                     size)
{
    int              comparisonWidth  = size * 4;  // 4 images side by side
    int              comparisonHeight = size;
    std::vector<RGB> comparisonImage(comparisonWidth * comparisonHeight, RGB(0, 0, 0));

    // Place images side by side
    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            // Original
            comparisonImage[y * comparisonWidth + x] = original[y * size + x];

            // Float result
            comparisonImage[y * comparisonWidth + (size + x)] = floatResult[y * size + x];

            // Posit result
            comparisonImage[y * comparisonWidth + (2 * size + x)] = positResult[y * size + x];

            // Quire result
            comparisonImage[y * comparisonWidth + (3 * size + x)] = quireResult[y * size + x];
        }
    }

    // Add vertical separator lines between images
    for (int y = 0; y < comparisonHeight; ++y)
    {
        for (int i = 1; i < 4; ++i)
        {
            int x                                    = i * size - 1;
            comparisonImage[y * comparisonWidth + x] = RGB(255, 0, 0);
        }
    }

    // Add labels at the top of each image section
    std::string originalLabel = "Original";
    std::string floatLabel    = "Float";
    std::string positLabel    = "Posit32";
    std::string quireLabel    = "Posit32+Quire";

    int labelY = 10;

    // This would actually render text, but for simplicity we'll just put colored rectangles
    for (int x = 0; x < size - 2; ++x)
    {
        comparisonImage[labelY * comparisonWidth + (size / 2 - originalLabel.length() * 3 + x)] =
            RGB(255, 255, 255);
        comparisonImage[labelY * comparisonWidth +
                        (size + size / 2 - floatLabel.length() * 3 + x)]     = RGB(255, 255, 255);
        comparisonImage[labelY * comparisonWidth +
                        (2 * size + size / 2 - positLabel.length() * 3 + x)] = RGB(255, 255, 255);
        comparisonImage[labelY * comparisonWidth +
                        (3 * size + size / 2 - quireLabel.length() * 3 + x)] = RGB(255, 255, 255);
    }

    return comparisonImage;
}

// Process an image with both float and posit FFT implementations
void processImageComparison()
{
    // Generate a test image
    std::vector<RGB> originalImage = generateTestImage(IMAGE_SIZE, IMAGE_SIZE);
    savePPM("original.ppm", originalImage, IMAGE_SIZE, IMAGE_SIZE);

    // Convert to grayscale for each numerical type
    std::vector<float>     floatGrayscale = convertToGrayscale<float>(originalImage);
    std::vector<posit32_t> positGrayscale = convertToGrayscale<posit32_t>(originalImage);

    // Convert grayscale data to complex format for FFT
    std::vector<Complex<float>>     floatComplex(IMAGE_SIZE * IMAGE_SIZE);
    std::vector<Complex<posit32_t>> positComplex(IMAGE_SIZE * IMAGE_SIZE);
    std::vector<Complex<posit32_t>> quireComplex(IMAGE_SIZE * IMAGE_SIZE);

    for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
    {
        floatComplex[i] = Complex<float>(floatGrayscale[i], 0);
        positComplex[i] = Complex<posit32_t>(positGrayscale[i], double_to_posit32(0));
        quireComplex[i] = Complex<posit32_t>(positGrayscale[i], double_to_posit32(0));
    }

    std::cout << "Starting FFT processing..." << std::endl;

    // Perform 2D FFT for float implementation
    auto startFloat = std::chrono::high_resolution_clock::now();

    // Perform row-wise 1D FFT
    std::vector<Complex<float>> floatFrequency(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<float>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = floatComplex[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<float>> transformedRow = fft(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            floatFrequency[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    // Perform column-wise 1D FFT
    std::vector<Complex<float>> floatFrequency2D(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<float>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = floatFrequency[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<float>> transformedCol = fft(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            floatFrequency2D[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Apply a filter in the frequency domain
    applyFilter(floatFrequency2D, IMAGE_SIZE);

    // Perform 2D IFFT for float implementation
    // First, column-wise IFFT
    std::vector<Complex<float>> floatSpatial(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<float>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = floatFrequency2D[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<float>> transformedCol = ifft(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            floatSpatial[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Then, row-wise IFFT
    std::vector<Complex<float>> floatReconstructed(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<float>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = floatSpatial[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<float>> transformedRow = ifft(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            floatReconstructed[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    auto endFloat = std::chrono::high_resolution_clock::now();
    auto floatDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endFloat - startFloat).count();

    // Perform 2D FFT for posit32 implementation
    auto startPosit = std::chrono::high_resolution_clock::now();

    // Perform row-wise 1D FFT
    std::vector<Complex<posit32_t>> positFrequency(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<posit32_t>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = positComplex[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedRow = fft(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            positFrequency[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    // Perform column-wise 1D FFT
    std::vector<Complex<posit32_t>> positFrequency2D(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<posit32_t>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = positFrequency[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedCol = fft(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            positFrequency2D[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Apply a filter in the frequency domain
    applyFilter(positFrequency2D, IMAGE_SIZE);

    // Perform 2D IFFT for posit32 implementation
    // First, column-wise IFFT
    std::vector<Complex<posit32_t>> positSpatial(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<posit32_t>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = positFrequency2D[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedCol = ifft(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            positSpatial[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Then, row-wise IFFT
    std::vector<Complex<posit32_t>> positReconstructed(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<posit32_t>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = positSpatial[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedRow = ifft(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            positReconstructed[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    auto endPosit = std::chrono::high_resolution_clock::now();
    auto positDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endPosit - startPosit).count();

    // Perform 2D FFT for posit32 with quire implementation
    auto startQuire = std::chrono::high_resolution_clock::now();

    // Perform row-wise 1D FFT with quire
    std::vector<Complex<posit32_t>> quireFrequency(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<posit32_t>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = quireComplex[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedRow = fftWithQuire(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            quireFrequency[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    // Perform column-wise 1D FFT with quire
    std::vector<Complex<posit32_t>> quireFrequency2D(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<posit32_t>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = quireFrequency[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedCol = fftWithQuire(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            quireFrequency2D[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Apply a filter in the frequency domain
    applyFilter(quireFrequency2D, IMAGE_SIZE);

    // Perform 2D IFFT for posit32 with quire implementation
    // First, column-wise IFFT with quire
    std::vector<Complex<posit32_t>> quireSpatial(IMAGE_SIZE * IMAGE_SIZE);
    for (int x = 0; x < IMAGE_SIZE; ++x)
    {
        std::vector<Complex<posit32_t>> col(IMAGE_SIZE);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            col[y] = quireFrequency2D[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedCol = ifftWithQuire(col);
        for (int y = 0; y < IMAGE_SIZE; ++y)
        {
            quireSpatial[y * IMAGE_SIZE + x] = transformedCol[y];
        }
    }

    // Then, row-wise IFFT with quire
    std::vector<Complex<posit32_t>> quireReconstructed(IMAGE_SIZE * IMAGE_SIZE);
    for (int y = 0; y < IMAGE_SIZE; ++y)
    {
        std::vector<Complex<posit32_t>> row(IMAGE_SIZE);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            row[x] = quireSpatial[y * IMAGE_SIZE + x];
        }

        std::vector<Complex<posit32_t>> transformedRow = ifftWithQuire(row);
        for (int x = 0; x < IMAGE_SIZE; ++x)
        {
            quireReconstructed[y * IMAGE_SIZE + x] = transformedRow[x];
        }
    }

    auto endQuire = std::chrono::high_resolution_clock::now();
    auto quireDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endQuire - startQuire).count();

    // Extract real components for visualization
    std::vector<float>     floatResult(IMAGE_SIZE * IMAGE_SIZE);
    std::vector<posit32_t> positResult(IMAGE_SIZE * IMAGE_SIZE);
    std::vector<posit32_t> quireResult(IMAGE_SIZE * IMAGE_SIZE);

    for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
    {
        floatResult[i] = floatReconstructed[i].real;
        positResult[i] = positReconstructed[i].real;
        quireResult[i] = quireReconstructed[i].real;
    }

    // Convert back to RGB images
    std::vector<RGB> floatImage = convertToRGB(floatResult);
    std::vector<RGB> positImage = convertToRGB(positResult);
    std::vector<RGB> quireImage = convertToRGB(quireResult);

    // Create error visualizations
    std::vector<RGB> floatError = visualizeError(originalImage, floatImage);
    std::vector<RGB> positError = visualizeError(originalImage, positImage);
    std::vector<RGB> quireError = visualizeError(originalImage, quireImage);

    // Calculate PSNR values
    double floatPSNR = calculatePSNR(originalImage, floatImage);
    double positPSNR = calculatePSNR(originalImage, positImage);
    double quirePSNR = calculatePSNR(originalImage, quireImage);

    // Create side-by-side comparison
    std::vector<RGB> comparisonImage =
        createComparisonImage(originalImage, floatImage, positImage, quireImage, IMAGE_SIZE);

    // Save all images
    savePPM("reconstructed_float.ppm", floatImage, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("reconstructed_posit.ppm", positImage, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("reconstructed_quire.ppm", quireImage, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("error_float.ppm", floatError, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("error_posit.ppm", positError, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("error_quire.ppm", quireError, IMAGE_SIZE, IMAGE_SIZE);
    savePPM("comparison.ppm", comparisonImage, IMAGE_SIZE * 4, IMAGE_SIZE);

    // Print performance and quality metrics
    std::cout << "Performance and Quality Metrics:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Float FFT: " << floatDuration << " ms, PSNR: " << floatPSNR << " dB" << std::endl;
    std::cout << "Posit FFT: " << positDuration << " ms, PSNR: " << positPSNR << " dB" << std::endl;
    std::cout << "Quire FFT: " << quireDuration << " ms, PSNR: " << quirePSNR << " dB" << std::endl;

    // Calculate PSNR improvement
    double positImprovement = positPSNR - floatPSNR;
    double quireImprovement = quirePSNR - floatPSNR;

    std::cout << "PSNR Improvement over float:" << std::endl;
    std::cout << "  Posit: " << positImprovement << " dB" << std::endl;
    std::cout << "  Quire: " << quireImprovement << " dB" << std::endl;
}

int main()
{
    try
    {
        std::cout << "Running Fourier Transform Precision Comparison..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        processImageComparison();

        auto                          end     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Example completed in " << elapsed.count() << " seconds." << std::endl;

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}