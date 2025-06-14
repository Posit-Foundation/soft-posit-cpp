#include <fstream>
#include <iostream>
#include <vector>

#include "softposit.h"

const int WIDTH          = 800;
const int HEIGHT         = 600;
const int MAX_ITERATIONS = 1000;

// Complex number operations using p16 posits
struct ComplexP16
{
    posit16_t real;
    posit16_t imag;

    ComplexP16(posit16_t r, posit16_t i) : real(r), imag(i)
    {
    }

    ComplexP16 operator*(const ComplexP16& other) const
    {
        // (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
        posit16_t ac = p16_mul(real, other.real);
        posit16_t bd = p16_mul(imag, other.imag);
        posit16_t ad = p16_mul(real, other.imag);
        posit16_t bc = p16_mul(imag, other.real);

        return ComplexP16(p16_sub(ac, bd), p16_add(ad, bc));
    }

    ComplexP16 operator+(const ComplexP16& other) const
    {
        return ComplexP16(p16_add(real, other.real), p16_add(imag, other.imag));
    }

    posit16_t magnitude() const
    {
        // sqrt(real^2 + imag^2)
        posit16_t real_squared = p16_mul(real, real);
        posit16_t imag_squared = p16_mul(imag, imag);
        posit16_t sum          = p16_add(real_squared, imag_squared);
        return p16_sqrt(sum);
    }
};

// Function to calculate the color based on the number of iterations
void getColor(int iterations, int& r, int& g, int& b)
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

    // Define the complex plane boundaries using p16
    posit16_t xMin = convertDoubleToP16(-2.0);
    posit16_t xMax = convertDoubleToP16(1.0);
    posit16_t yMin = convertDoubleToP16(-1.5);
    posit16_t yMax = convertDoubleToP16(1.5);

    // Constants for calculations
    posit16_t width_p16  = convertDoubleToP16(WIDTH);
    posit16_t height_p16 = convertDoubleToP16(HEIGHT);
    posit16_t two_p16    = convertDoubleToP16(2.0);

    // Generate the Mandelbrot set
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            // Map pixel coordinates to complex plane using p16
            posit16_t x_p16 = convertDoubleToP16(x);
            posit16_t y_p16 = convertDoubleToP16(y);

            posit16_t real = p16_add(xMin, p16_mul(p16_sub(xMax, xMin), p16_div(x_p16, width_p16)));
            posit16_t imag =
                p16_add(yMin, p16_mul(p16_sub(yMax, yMin), p16_div(y_p16, height_p16)));

            ComplexP16 c(real, imag);
            ComplexP16 z(convertDoubleToP16(0.0), convertDoubleToP16(0.0));

            int iterations = 0;

            // Iterate the Mandelbrot formula
            while (p16_lt(z.magnitude(), two_p16) && iterations < MAX_ITERATIONS)
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
    std::ofstream file("posit_mandelbrot.ppm");
    if (!file)
    {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    // Write PPM header
    file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

    // Write image data
    file.write(reinterpret_cast<const char*>(image.data()), image.size());

    std::cout << "Mandelbrot Set generated using p16 posits and saved to posit_mandelbrot.ppm"
              << std::endl;
    std::cout << "You can view this file using any image viewer that supports PPM format."
              << std::endl;

    return 0;
}