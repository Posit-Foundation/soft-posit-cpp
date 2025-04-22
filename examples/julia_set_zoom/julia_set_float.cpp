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

// Complex number operations using doubles
struct Complex {
  double real;
  double imag;

  Complex(double r, double i) : real(r), imag(i) {}

  Complex operator*(const Complex &other) const {
    // (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
    double ac = real * other.real;
    double bd = imag * other.imag;
    double ad = real * other.imag;
    double bc = imag * other.real;

    return Complex(ac - bd, ad + bc);
  }

  Complex operator+(const Complex &other) const {
    return Complex(real + other.real, imag + other.imag);
  }

  double magnitudeSquared() const {
    // real^2 + imag^2
    return real * real + imag * imag;
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

  // Julia set constant
  Complex c(cReal, cImag);

  // Process each pixel
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // Map pixel coordinates to complex plane
      double real = xMin + (x / static_cast<double>(WIDTH)) * (xMax - xMin);
      double imag = yMin + (y / static_cast<double>(HEIGHT)) * (yMax - yMin);

      Complex z(real, imag);

      int iterations = 0;
      double smoothColor = 0.0;

      // Iterate the Julia set formula: z = z² + c
      while (z.magnitudeSquared() < 4.0 && iterations < MAX_ITERATIONS) {
        z = z * z + c;
        iterations++;
      }

      // Calculate smooth coloring using an alternative approach without log
      if (iterations < MAX_ITERATIONS) {
        // Get final magnitude
        double mag = sqrt(z.magnitudeSquared());

        // Alternative smooth coloring formula using the final magnitude
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

  std::cout << "Julia Set (float) generated at zoom level " << zoomFactor
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
  std::string filename = "julia_float_zoom_" +
                         std::to_string(static_cast<int>(zoomFactor)) + ".ppm";
  generateJuliaSet(zoomFactor, centerX, centerY, cReal, cImag, filename);

  std::cout << "Julia Set generation complete." << std::endl;
  std::cout << "You can view the PPM file using any image viewer that supports "
               "PPM format."
            << std::endl;

  return 0;
}