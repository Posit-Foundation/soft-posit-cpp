#!/usr/bin/env python
"""
Example script demonstrating the use of the SoftPosit library in Python.
This demonstrates the various posit number formats and operations.
"""

from __future__ import print_function, division
import sys
import os
import numpy as np
import matplotlib.pyplot as plt

# Try importing the required modules
try:
    import softposit

    print("Successfully imported softposit module")
except ImportError as e:
    print(f"Error importing softposit module: {e}")
    sys.exit(1)


def demo_basic_operations():
    """Demonstrate basic operations with different posit formats."""
    print("\n==== Basic Posit Operations ====")

    # Create posit values
    p8_a = softposit.posit8(1.5)
    p8_b = softposit.posit8(2.25)

    p16_a = softposit.posit16(1.5)
    p16_b = softposit.posit16(2.25)

    p32_a = softposit.posit32(1.5)
    p32_b = softposit.posit32(2.25)

    print(f"\nposit8 precision:")
    print(f"  p8_a = {p8_a}")
    print(f"  p8_b = {p8_b}")
    print(f"  p8_a + p8_b = {p8_a + p8_b}")
    print(f"  p8_a - p8_b = {p8_a - p8_b}")
    print(f"  p8_a * p8_b = {p8_a * p8_b}")
    print(f"  p8_a / p8_b = {p8_a / p8_b}")

    print(f"\nposit16 precision:")
    print(f"  p16_a = {p16_a}")
    print(f"  p16_b = {p16_b}")
    print(f"  p16_a + p16_b = {p16_a + p16_b}")
    print(f"  p16_a - p16_b = {p16_a - p16_b}")
    print(f"  p16_a * p16_b = {p16_a * p16_b}")
    print(f"  p16_a / p16_b = {p16_a / p16_b}")

    print(f"\nposit32 precision:")
    print(f"  p32_a = {p32_a}")
    print(f"  p32_b = {p32_b}")
    print(f"  p32_a + p32_b = {p32_a + p32_b}")
    print(f"  p32_a - p32_b = {p32_a - p32_b}")
    print(f"  p32_a * p32_b = {p32_a * p32_b}")
    print(f"  p32_a / p32_b = {p32_a / p32_b}")

    # Type conversions
    print("\n==== Type Conversions ====")
    print(f"  float(p32_a) = {float(p32_a)}")
    print(f"  int(p32_a) = {int(p32_a)}")
    print(f"  softposit.posit8(p32_a) = {softposit.posit8(float(p32_a))}")
    print(f"  softposit.posit16(p8_a) = {softposit.posit16(float(p8_a))}")


def demo_precision_effects():
    """Demonstrate the effects of different precision levels."""
    print("\n==== Precision Effects ====")

    # Test a range of values
    value = 1.0 / 3.0  # A value that can't be exactly represented in binary

    p8 = softposit.posit8(value)
    p16 = softposit.posit16(value)
    p32 = softposit.posit32(value)

    print(f"Original value: {value}")
    print(f"posit8:  {p8}  (error: {abs(float(p8) - value)})")
    print(f"posit16: {p16} (error: {abs(float(p16) - value)})")
    print(f"posit32: {p32} (error: {abs(float(p32) - value)})")

    # Gradual underflow test
    print("\nGradual underflow test:")
    for exp in range(0, -10, -1):
        value = 2.0**exp
        p8 = softposit.posit8(value)
        p16 = softposit.posit16(value)
        p32 = softposit.posit32(value)

        print(f"2^{exp} = {value}")
        print(f"  posit8:  {p8}")
        print(f"  posit16: {p16}")
        print(f"  posit32: {p32}")


def demo_accumulation():
    """Demonstrate accumulation and errors."""
    print("\n==== Accumulation Demo ====")

    # Sum of many small numbers
    count = 1000
    small = 1e-4

    # Using float64
    float_sum = 0.0
    for _ in range(count):
        float_sum += small

    # Using posit8
    p8_sum = softposit.posit8(0.0)
    for _ in range(count):
        p8_sum += small

    # Using posit16
    p16_sum = softposit.posit16(0.0)
    for _ in range(count):
        p16_sum += small

    # Using posit32
    p32_sum = softposit.posit32(0.0)
    for _ in range(count):
        p32_sum += small

    exact = count * small
    print(f"Adding {count} values of {small}")
    print(f"Exact result: {exact}")
    print(f"float64: {float_sum}, error: {abs(float_sum - exact)}")
    print(f"posit8:  {p8_sum}, error: {abs(float(p8_sum) - exact)}")
    print(f"posit16: {p16_sum}, error: {abs(float(p16_sum) - exact)}")
    print(f"posit32: {p32_sum}, error: {abs(float(p32_sum) - exact)}")


def plot_precision_comparison():
    """Plot comparisons between different posit formats (when numpy/matplotlib are available)."""

    print("\n==== Plotting Precision Comparison ====")

    # Generate values
    x_values = np.linspace(0, 10, 1000)
    y_exact = np.sin(x_values)

    # Convert to posit and back
    y_p8 = np.array([float(softposit.posit8(v)) for v in y_exact])
    y_p16 = np.array([float(softposit.posit16(v)) for v in y_exact])
    y_p32 = np.array([float(softposit.posit32(v)) for v in y_exact])

    # Calculate absolute errors
    err_p8 = np.abs(y_p8 - y_exact)
    err_p16 = np.abs(y_p16 - y_exact)
    err_p32 = np.abs(y_p32 - y_exact)

    # Create plots
    plt.figure(figsize=(12, 10))

    # Plot 1: Values
    plt.subplot(2, 1, 1)
    plt.plot(x_values, y_exact, "k-", label="Exact (float64)")
    plt.plot(x_values, y_p8, "r-", alpha=0.7, label="posit8")
    plt.plot(x_values, y_p16, "g-", alpha=0.7, label="posit16")
    plt.plot(x_values, y_p32, "b-", alpha=0.7, label="posit32")
    plt.title("sin(x) with Different Posit Formats")
    plt.xlabel("x")
    plt.ylabel("sin(x)")
    plt.legend()
    plt.grid(True)

    # Plot 2: Errors
    plt.subplot(2, 1, 2)
    plt.semilogy(x_values, err_p8, "r-", label="posit8 error")
    plt.semilogy(x_values, err_p16, "g-", label="posit16 error")
    plt.semilogy(x_values, err_p32, "b-", label="posit32 error")
    plt.title("Absolute Error in sin(x) Approximation")
    plt.xlabel("x")
    plt.ylabel("Absolute Error (log scale)")
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.savefig("posit_precision_comparison.png")
    print("Plot saved as 'posit_precision_comparison.png'")


def main():
    """Main function to run the demos."""
    print("SoftPosit Python Example")
    print("=" * 40)

    demo_basic_operations()
    demo_precision_effects()
    demo_accumulation()

    
    plot_precision_comparison()

    print("\nExample completed successfully!")


if __name__ == "__main__":
    main()
