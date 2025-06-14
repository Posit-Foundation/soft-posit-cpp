#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

const int WIDTH          = 800;
const int HEIGHT         = 600;
const int MAX_ITERATIONS = 1000;

// Complex number operations without using std::complex
struct Complex
{
    double real;
    double imag;

    Complex(double r, double i) : real(r), imag(i)
    {
    }

    Complex operator*(const Complex &other) const
    {
        return Complex(real * other.real - imag * other.imag,
                       real * other.imag + imag * other.real);
    }

    Complex operator+(const Complex &other) const
    {
        return Complex(real + other.real, imag + other.imag);
    }

    double magnitude() const
    {
        return std::sqrt(real * real + imag * imag);
    }
};

// Function to calculate the color based on the number of iterations
void getColor(int iterations, int &r, int &g, int &b)
{
    if (iterations == MAX_ITERATIONS)
    {
        r = g = b = 0;  // Black
        return;
    }

    // Create a smooth color gradient
    double t = static_cast<double>(iterations) / MAX_ITERATIONS;
    r        = static_cast<int>(9 * (1 - t) * t * t * t * 255);
    g        = static_cast<int>(15 * (1 - t) * (1 - t) * t * t * 255);
    b        = static_cast<int>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
}

int main()
{
    // Create a buffer to store the image data
    std::vector<unsigned char> image(WIDTH * HEIGHT * 3);

    // Define the complex plane boundaries
    double xMin = -2.0;
    double xMax = 1.0;
    double yMin = -1.5;
    double yMax = 1.5;

    // Generate the Mandelbrot set
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            // Map pixel coordinates to complex plane
            double real = xMin + (xMax - xMin) * x / WIDTH;
            double imag = yMin + (yMax - yMin) * y / HEIGHT;

            Complex c(real, imag);
            Complex z(0, 0);

            int iterations = 0;

            // Iterate the Mandelbrot formula
            while (z.magnitude() < 2.0 && iterations < MAX_ITERATIONS)
            {
                z = z * z + c;
                iterations++;
            }

            // Calculate color
            int r, g, b;
            getColor(iterations, r, g, b);

            // Store the color in the image buffer
            int index        = (y * WIDTH + x) * 3;
            image[index]     = r;  // Red
            image[index + 1] = g;  // Green
            image[index + 2] = b;  // Blue
        }
    }

    // Write the image to a PPM file
    std::ofstream file("mandelbrot.ppm");
    if (!file)
    {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    // Write PPM header
    file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

    // Write image data
    file.write(reinterpret_cast<const char *>(image.data()), image.size());

    std::cout << "Mandelbrot Set generated and saved to mandelbrot.ppm" << std::endl;
    std::cout << "You can view this file using any image viewer that supports "
                 "PPM format."
              << std::endl;

    return 0;
}