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

### Julia Set Zoom Comparison

This example visually demonstrates the precision advantages of posit arithmetic compared to traditional floating-point when generating Julia set fractals at high zoom levels.

To run:
```bash
cd build/julia_set_zoom
./julia_set_comparison
```

This will generate multiple comparison images at different zoom levels, showing how posits maintain precision in deep zooms where floating-point begins to fail.

See [julia_set_zoom/README.md](julia_set_zoom/README.md) for more details.

### Matrix Multiplication Precision Comparison

This example demonstrates the precision advantages of posit arithmetic compared to traditional floating-point when performing matrix multiplication with ill-conditioned matrices.

To run:
```bash
cd build/matrix_precision
./matrix_precision_dynamic
```

The program generates heatmap images that visualize where precision differences occur between:
- Standard floating-point (float)
- POSIT arithmetic (posit32)
- POSIT with quire accumulation (maximum precision)

See [matrix_precision/README.md](matrix_precision/README.md) for more details.

## Viewing PPM Images

All examples produce PPM image files. You can view these with:

1. Many image viewers support PPM format directly
2. Use ImageMagick to convert: `convert image.ppm image.png`
3. Use online PPM viewers

## License

All examples are part of the SoftPosit project and are subject to the same license terms.
