#include "softposit.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Define the image dimensions
const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_ITERATIONS = 1000;

// Julia set parameters
const double DEFAULT_ZOOM = 1.0;
const double DEFAULT_CENTER_X = 0.0;
const double DEFAULT_CENTER_Y = 0.0;
const double DEFAULT_C_REAL = -0.8;
const double DEFAULT_C_IMAG = 0.156;

// Complex number operations using p32 posits for high precision
struct ComplexP32 {
  posit32_t real;
  posit32_t imag;

  ComplexP32(posit32_t r, posit32_t i) : real(r), imag(i) {}

  ComplexP32 operator*(const ComplexP32 &other) const {
    // (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
    posit32_t ac = p32_mul(real, other.real);
    posit32_t bd = p32_mul(imag, other.imag);
    posit32_t ad = p32_mul(real, other.imag);
    posit32_t bc = p32_mul(imag, other.real);

    return ComplexP32(p32_sub(ac, bd), p32_add(ad, bc));
  }

  ComplexP32 operator+(const ComplexP32 &other) const {
    return ComplexP32(p32_add(real, other.real), p32_add(imag, other.imag));
  }

  posit32_t magnitudeSquared() const {
    // real^2 + imag^2
    posit32_t real_squared = p32_mul(real, real);
    posit32_t imag_squared = p32_mul(imag, imag);
    return p32_add(real_squared, imag_squared);
  }
};

// Function to calculate the color based on the number of iterations
void getColor(int iterations, double smoothColor, int &r, int &g, int &b) {
  if (iterations == MAX_ITERATIONS) {
    r = g = b = 0; // Black
    return;
  }

  // Create a smooth color gradient using a modified version of the "hot"
  // colormap
  double t = smoothColor;

  // Oscillate between different hues
  double freq = 0.1;
  double phase1 = 0.0;
  double phase2 = 2.0;
  double phase3 = 4.0;

  r = static_cast<int>(255 * 0.5 * (1 + sin(freq * t + phase1)));
  g = static_cast<int>(255 * 0.5 * (1 + sin(freq * t + phase2)));
  b = static_cast<int>(255 * 0.5 * (1 + sin(freq * t + phase3)));
}

// Generate Julia set at a specific zoom level and center point
void generateJuliaSet(double zoomFactor, double centerX, double centerY,
                      double cReal, double cImag, const std::string &filename) {
  // Create a buffer to store the image data
  std::vector<unsigned char> image(WIDTH * HEIGHT * 3);

  // Calculate the range for this zoom level
  double rangeX = 4.0 / zoomFactor;
  double rangeY = 4.0 / zoomFactor;

  // Calculate bounds
  double xMin = centerX - rangeX / 2;
  double xMax = centerX + rangeX / 2;
  double yMin = centerY - rangeY / 2;
  double yMax = centerY + rangeY / 2;

  // Convert Julia set constant c to posit32
  posit32_t c_real = convertDoubleToP32(cReal);
  posit32_t c_imag = convertDoubleToP32(cImag);
  ComplexP32 c(c_real, c_imag);

  // Posit constants
  posit32_t four_p32 = convertDoubleToP32(4.0);

  // Process each pixel
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // Map pixel coordinates to complex plane
      double real_d = xMin + (x / static_cast<double>(WIDTH)) * (xMax - xMin);
      double imag_d = yMin + (y / static_cast<double>(HEIGHT)) * (yMax - yMin);

      // Convert to posit32
      posit32_t real = convertDoubleToP32(real_d);
      posit32_t imag = convertDoubleToP32(imag_d);

      ComplexP32 z(real, imag);

      int iterations = 0;
      double smoothColor = 0.0;

      // Iterate the Julia set formula: z = z² + c
      while (p32_lt(z.magnitudeSquared(), four_p32) &&
             iterations < MAX_ITERATIONS) {
        z = z * z + c;
        iterations++;
      }

      // Calculate smooth coloring
      if (iterations < MAX_ITERATIONS) {
        // Get final magnitude squared using posit operations
        posit32_t magSquared = z.magnitudeSquared();

        // Convert to double for final calculation
        double mag = sqrt(convertP32ToDouble(magSquared));

        // Alternative smooth coloring formula that doesn't require log
        // This uses normalized iteration count plus a fractional part based on
        // final magnitude
        double nu = iterations + 1 - (mag - 2.0) / (4.0 - 2.0);

        // Multiply by a factor to get good color distribution
        smoothColor = nu * 10.0;
      } else {
        smoothColor = iterations;
      }

      // Calculate color
      int r, g, b;
      getColor(iterations, smoothColor, r, g, b);

      // Store the color in the image buffer
      int index = (y * WIDTH + x) * 3;
      image[index] = r;     // Red
      image[index + 1] = g; // Green
      image[index + 2] = b; // Blue
    }
  }

  // Write the image to a PPM file
  std::ofstream file(filename);
  if (!file) {
    std::cerr << "Error: Could not create output file." << std::endl;
    return;
  }

  // Write PPM header
  file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

  // Write image data
  file.write(reinterpret_cast<const char *>(image.data()), image.size());

  std::cout << "Julia Set (posit) generated at zoom level " << zoomFactor
            << " and saved to " << filename << std::endl;
}

int main(int argc, char *argv[]) {
  double zoomFactor = DEFAULT_ZOOM;
  double centerX = DEFAULT_CENTER_X;
  double centerY = DEFAULT_CENTER_Y;
  double cReal = DEFAULT_C_REAL;
  double cImag = DEFAULT_C_IMAG;

  // Parse command line arguments
  if (argc > 1)
    zoomFactor = std::stod(argv[1]);
  if (argc > 2)
    centerX = std::stod(argv[2]);
  if (argc > 3)
    centerY = std::stod(argv[3]);
  if (argc > 4)
    cReal = std::stod(argv[4]);
  if (argc > 5)
    cImag = std::stod(argv[5]);

  // Generate images at different zoom levels
  std::string filename = "julia_posit_zoom_" +
                         std::to_string(static_cast<int>(zoomFactor)) + ".ppm";
  generateJuliaSet(zoomFactor, centerX, centerY, cReal, cImag, filename);

  std::cout << "Julia Set generation complete." << std::endl;
  std::cout << "You can view the PPM file using any image viewer that supports "
               "PPM format."
            << std::endl;

  return 0;
}