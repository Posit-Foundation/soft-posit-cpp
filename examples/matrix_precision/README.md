# Matrix Multiplication Precision Comparison

This example demonstrates the precision advantages of POSIT numbers compared to traditional floating-point arithmetic for matrix multiplication operations.

## What This Example Shows

Matrix multiplication is a fundamental operation in scientific computing, machine learning, and many other fields. It's also an operation where precision errors can accumulate and lead to significant inaccuracies, especially with:

1. Ill-conditioned matrices
2. Matrices with very small and very large values mixed together
3. Long chains of calculations where errors propagate

This example visualizes these precision differences by:

1. Generating two matrices with deliberately challenging values (mixing small and large numbers)
2. Multiplying them using three different approaches:
   - Standard floating-point (float)
   - POSIT arithmetic (posit32)
   - POSIT with quire accumulation for maximum precision

The program produces heatmap images highlighting where the three approaches differ in their results. Areas with larger differences (shown in red) indicate where traditional floating-point struggles with precision, while POSIT maintains accuracy.

## The Quire Advantage

One of the unique features of the POSIT number format is the "quire" - a fixed-size accumulator that can exactly store dot products without intermediate rounding errors. This is particularly valuable for matrix multiplication, where each result cell is a dot product of a row and column.

The example demonstrates this advantage by showing the difference between:
- Regular POSIT arithmetic (already more precise than float)
- POSIT with quire accumulation (maximum precision)

## Running the Example

To build and run the example:

```bash
# Navigate to the build directory
cd build/matrix_precision

# Run the program
./matrix_precision_dynamic
```

This will generate several PPM image files:
- `matrix_diff_float_posit.ppm` - Shows differences between float and posit32 results
- `matrix_diff_float_quire.ppm` - Shows differences between float and posit32+quire results
- `matrix_diff_posit_quire.ppm` - Shows differences between posit32 and posit32+quire results
- `matrix_heatmap_legend.ppm` - Color legend for interpreting the heatmaps

## Interpreting the Results

In the heatmap images:
- **Blue areas** represent small differences (high agreement between number formats)
- **White areas** represent moderate differences
- **Red areas** represent large differences (significant precision issues)

The console output also shows the mean absolute difference between the different approaches, providing a quantitative measure of the precision advantages.

## Why This Matters

In real-world applications like:
- Scientific simulations
- Machine learning
- Financial calculations
- Signal processing

These precision differences can be the difference between correct and incorrect results, especially as the size of matrices and number of calculations increases.

POSIT's design, especially when using the quire accumulator, provides a robust solution to these precision challenges while maintaining reasonable performance and storage requirements. 