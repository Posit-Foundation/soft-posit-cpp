# Multiplication Closure for Posit Numbers

This example demonstrates the multiplication closure properties of the Posit numerical format, showing how it behaves differently from traditional floating-point.

## What is Multiplication Closure?

Multiplication closure refers to whether a number system is "closed" under multiplication - meaning, when you multiply any two numbers in the system, is the result always another number in the system?

The visualization created by this example shows:
- Black regions: Exact multiplication results
- Purple regions: Inexact multiplication results (still valid Posits)
- Yellow dots: NaN results (only occurs in specific cases like ∞ × 0)

## Statistics

Typical multiplication closure results for Posit16:
- About 18% exact multiplications
- About 82% inexact multiplications
- Approximately 0.003% NaN results (only from specific operations like ∞ × 0)
- No overflow or underflow (a key advantage of Posits over IEEE floating-point)

## Building and Running

To build this example:

```bash
mkdir -p build
cd build
cmake ..
make
```

Then run the executable:

```bash
./multiplication_closure_dynamic  # or _static for the statically linked version
```

## Output

The program will generate:
1. A PPM image file showing the multiplication closure visualization
2. Statistics showing the percentage breakdown of result types

You can view the PPM file with many image viewers, or convert it to another format:
```bash
convert multiplication_closure.ppm multiplication_closure.png  # Using ImageMagick
``` 