# Mandelbrot Set Examples

This directory contains two implementations of the Mandelbrot Set:

1. A standard C++ implementation using double precision
2. A posit-based implementation using p16 (16-bit) posits

## Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- The SoftPosit library (included in this repository)

## Building the Examples

1. First, ensure you're in the mandelbrot example directory:

```bash
cd examples/mandelbrot
```

2. Create a build directory and navigate to it:

```bash
mkdir build
cd build
```

3. Configure the project with CMake:

```bash
cmake ..
```

4. Build the examples:

```bash
make
```

This will create two executables:

- `mandelbrot`: The standard double-precision implementation
- `posit_mandelbrot`: The p16 posit-based implementation

## Running the Examples

### Standard Implementation

```bash
./mandelbrot
```

This will generate a PPM file named `mandelbrot.ppm`.

### Posit Implementation

```bash
./posit_mandelbrot
```

This will generate a PPM file named `posit_mandelbrot.ppm`.

## Viewing the Results

Both implementations generate PPM (Portable Pixmap) format images that can be viewed with any image viewer that supports this format. Common viewers include:

- ImageMagick (`display` command)
- GIMP
- Most web browsers

## Implementation Details

### Standard Implementation

- Uses double-precision floating-point arithmetic
- Implements complex number operations using standard C++ math
- Generates a 800x600 image with smooth color gradients

### Posit Implementation

- Uses 16-bit posits (p16) for all calculations
- Implements complex number operations using the SoftPosit library
- Maintains the same image dimensions and color scheme as the standard implementation
- Demonstrates the use of posit arithmetic for fractal generation

## Troubleshooting

If you encounter build issues:

1. Ensure you have all prerequisites installed
2. Make sure you're in the correct directory when running CMake
3. Check that the SoftPosit library is properly built
4. Verify that your compiler supports C++17

## License

This example is part of the SoftPosit project and is subject to the same license terms.
