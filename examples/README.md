# SoftPosit C++ Examples

This directory contains examples demonstrating the use of the SoftPosit library with C++.

## Building the Examples

To build all examples, follow these steps:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_PREFIX_PATH=../../install
make
```

The `-DCMAKE_PREFIX_PATH` option should point to the directory where SoftPosit is installed.

## Available Examples

### Mandelbrot Set

This example demonstrates using Posit arithmetic to render the Mandelbrot set.

To run:
```bash
cd build/mandelbrot
./posit_mandelbrot_dynamic
```

See [mandelbrot/README.md](mandelbrot/README.md) for more details.

### Multiplication Closure

This example visualizes the multiplication closure properties of Posit numbers, showing:
- Where multiplications produce exact results
- Where they produce inexact but valid results
- Special cases that produce NaN

To run:
```bash
cd build/multiplication_closure
./multiplication_closure_dynamic
```

See [multiplication_closure/README.md](multiplication_closure/README.md) for more details.

## Viewing PPM Images

Both examples produce PPM image files. You can view these with:

1. Many image viewers support PPM format directly
2. Use ImageMagick to convert: `convert image.ppm image.png`
3. Use online PPM viewers

## License

All examples are part of the SoftPosit project and are subject to the same license terms.
