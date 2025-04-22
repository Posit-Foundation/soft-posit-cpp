# Fourier Transform Image Processing

This example demonstrates the precision advantages of POSIT numbers when performing Fast Fourier Transform (FFT) and Inverse Fast Fourier Transform (IFFT) operations on images.

## What This Example Shows

Fourier transforms are fundamental operations in signal and image processing, but they are also computationally intensive and highly sensitive to numerical precision. This example demonstrates:

1. How POSIT arithmetic provides better precision for FFT and IFFT operations
2. How the accumulation of errors during forward and inverse transforms affects image quality
3. The comparative advantages of using quire for complex multiply-accumulate operations

## Technical Details

The example implements:

1. A basic FFT/IFFT algorithm using:
   - Standard floating-point (float)
   - POSIT arithmetic (posit32)
   - POSIT with quire accumulation

2. Image processing operations:
   - Forward transform of a test image
   - Filter operations in the frequency domain
   - Inverse transform back to the spatial domain

3. Comparison metrics:
   - Direct pixel-by-pixel comparison between original and reconstructed images
   - Signal-to-noise ratio (SNR) measurements
   - Error distribution visualizations

## Running the Example

To build and run the example:

```bash
# Navigate to the build directory
cd build/fourier_transform

# Run the program
./fourier_transform_dynamic
```

This will generate several visualization files:
- `original.ppm` - The original test image
- `reconstructed_float.ppm` - Image after FFT and IFFT using float arithmetic
- `reconstructed_posit.ppm` - Image after FFT and IFFT using posit32 arithmetic
- `reconstructed_quire.ppm` - Image after FFT and IFFT using posit32 with quire accumulation
- `error_float.ppm` - Visualization of the error in the float implementation
- `error_posit.ppm` - Visualization of the error in the posit implementation
- `error_quire.ppm` - Visualization of the error in the posit+quire implementation
- `comparison.ppm` - Side-by-side comparison of all results

## Interpreting the Results

The most visible effects typically occur in:

1. **Fine details and sharp edges**: These require high-frequency components that are most susceptible to precision errors.
2. **Smooth gradients**: Areas where small errors can create visible banding or artifacts.
3. **Overall image quality**: Measured by SNR and visual inspection.

Notice how the float implementation may show:
- Ringing artifacts (Gibbs phenomenon)
- Loss of fine detail
- Numerical noise in reconstructed images

The POSIT implementation, especially with quire accumulation, should show better preservation of detail and fewer artifacts.

## Real-world Applications

The precision advantages demonstrated here directly apply to:
- Medical imaging (MRI, CT scans)
- Digital signal processing
- Image compression algorithms
- Scientific computing and spectral analysis

In these applications, improved precision can mean better diagnostic quality, more accurate results, or better compression efficiency. 