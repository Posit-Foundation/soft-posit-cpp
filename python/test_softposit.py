#!/usr/bin/env python
# Test script for the SoftPosit Python wrapper

from __future__ import print_function, division
import softposit
import sys

# Check Python version for string formatting
PY3 = sys.version_info[0] >= 3


def format_str(template, *args, **kwargs):
    """
    Helper function to handle string formatting in both Python 2 and 3.
    """
    if PY3:
        return template.format(*args, **kwargs)
    else:
        # Simple replacement for basic cases in Python 2
        result = template
        for i, arg in enumerate(args):
            result = result.replace("{" + str(i) + "}", str(arg))
        for key, value in kwargs.items():
            result = result.replace("{" + key + "}", str(value))
        return result


def test_posit8():
    """Test basic posit8 functionality"""
    print("Testing posit8...")

    # Create posit values
    a = softposit.posit8(1.5)
    b = softposit.posit8(0.5)

    # Test arithmetic
    assert float(a + b) == 2.0, format_str("1.5 + 0.5 = {0}, expected 2.0", a + b)
    assert float(a - b) == 1.0, format_str("1.5 - 0.5 = {0}, expected 1.0", a - b)
    assert float(a * b) == 0.75, format_str("1.5 * 0.5 = {0}, expected 0.75", a * b)
    assert float(a / b) == 3.0, format_str("1.5 / 0.5 = {0}, expected 3.0", a / b)

    # Test comparison
    assert a > b, "Expected 1.5 > 0.5"
    assert a >= b, "Expected 1.5 >= 0.5"
    assert b < a, "Expected 0.5 < 1.5"
    assert b <= a, "Expected 0.5 <= 1.5"
    assert a != b, "Expected 1.5 != 0.5"

    # Test conversion
    c = softposit.posit8(1)
    assert int(c) == 1, format_str("int(posit8(1)) = {0}, expected 1", int(c))
    assert float(c) == 1.0, format_str("float(posit8(1)) = {0}, expected 1.0", float(c))

    # Test NaR
    nar = softposit.posit8(0).toNaR()
    assert nar.isNaR(), "Expected NaR value to have isNaR() == True"

    print("posit8 tests passed!")


def test_posit16():
    """Test basic posit16 functionality"""
    print("Testing posit16...")

    # Create posit values
    a = softposit.posit16(1.5)
    b = softposit.posit16(0.5)

    # Test arithmetic
    assert abs(float(a + b) - 2.0) < 1e-6, format_str(
        "1.5 + 0.5 = {0}, expected 2.0", a + b
    )
    assert abs(float(a - b) - 1.0) < 1e-6, format_str(
        "1.5 - 0.5 = {0}, expected 1.0", a - b
    )
    assert abs(float(a * b) - 0.75) < 1e-6, format_str(
        "1.5 * 0.5 = {0}, expected 0.75", a * b
    )
    assert abs(float(a / b) - 3.0) < 1e-6, format_str(
        "1.5 / 0.5 = {0}, expected 3.0", a / b
    )

    # Test fma
    c = a.fma(b, softposit.posit16(1.0))
    assert abs(float(c) - 1.75) < 1e-6, format_str(
        "1.5 fma(0.5, 1.0) = {0}, expected 1.75", c
    )

    # Test sqrt
    d = softposit.posit16(4.0)
    sqrt_d = d.sqrt()
    assert abs(float(sqrt_d) - 2.0) < 1e-6, format_str(
        "sqrt(4.0) = {0}, expected 2.0", sqrt_d
    )

    # Test conversions
    e = a.toPosit8()
    assert type(e).__name__ == "posit8", format_str(
        "Expected posit8 type, got {0}", type(e).__name__
    )

    f = a.toPosit32()
    assert type(f).__name__ == "posit32", format_str(
        "Expected posit32 type, got {0}", type(f).__name__
    )

    print("posit16 tests passed!")


def test_posit32():
    """Test basic posit32 functionality"""
    print("Testing posit32...")

    # Create posit values
    a = softposit.posit32(1.5)
    b = softposit.posit32(0.5)

    # Test arithmetic
    assert abs(float(a + b) - 2.0) < 1e-10, format_str(
        "1.5 + 0.5 = {0}, expected 2.0", a + b
    )
    assert abs(float(a - b) - 1.0) < 1e-10, format_str(
        "1.5 - 0.5 = {0}, expected 1.0", a - b
    )
    assert abs(float(a * b) - 0.75) < 1e-10, format_str(
        "1.5 * 0.5 = {0}, expected 0.75", a * b
    )
    assert abs(float(a / b) - 3.0) < 1e-10, format_str(
        "1.5 / 0.5 = {0}, expected 3.0", a / b
    )

    # Test conversions
    e = a.toPosit8()
    assert type(e).__name__ == "posit8", format_str(
        "Expected posit8 type, got {0}", type(e).__name__
    )

    f = a.toPosit16()
    assert type(f).__name__ == "posit16", format_str(
        "Expected posit16 type, got {0}", type(f).__name__
    )

    print("posit32 tests passed!")


def test_quire():
    """Test quire functionality"""
    print("Testing quire16...")

    # Create quire and posit values
    q = softposit.quire16()
    a = softposit.posit16(1.5)
    b = softposit.posit16(0.5)

    # Start with an empty quire
    q.clr()
    assert not q.isNaR(), "Expected cleared quire to not be NaR"

    # Add product a*b to quire
    q.qma(a, b)
    result = q.toPosit()
    assert abs(float(result) - 0.75) < 1e-6, format_str(
        "q.qma(1.5, 0.5) = {0}, expected 0.75", result
    )

    # Add another product
    q.qma(a, a)
    result = q.toPosit()
    assert abs(float(result) - 3.0) < 1e-6, format_str(
        "After second qma, q = {0}, expected 3.0", result
    )

    # Clear the quire
    q.clr()
    result = q.toPosit()
    assert float(result) == 0.0, format_str("Cleared quire = {0}, expected 0.0", result)

    print("quire16 tests passed!")


def test_dynamic_posit():
    """Test dynamic posit functionality"""
    print("Testing dynamic posit...")

    # Create posit values
    a = softposit.posit_2(1.5, 8)  # 8-bit posit with es=2
    b = softposit.posit_2(0.5, 8)

    # Test arithmetic
    c = a + b
    assert abs(float(c) - 2.0) < 1e-3, format_str("1.5 + 0.5 = {0}, expected 2.0", c)

    # Test conversion to different bit widths
    d = a.toPosit_2(16)  # 16-bit posit
    assert d.x == 16, format_str("Expected bit width 16, got {0}", d.x)

    # Convert to standard posit types
    e = a.toPosit8()
    assert type(e).__name__ == "posit8", format_str(
        "Expected posit8 type, got {0}", type(e).__name__
    )

    f = a.toPosit16()
    assert type(f).__name__ == "posit16", format_str(
        "Expected posit16 type, got {0}", type(f).__name__
    )

    g = a.toPosit32()
    assert type(g).__name__ == "posit32", format_str(
        "Expected posit32 type, got {0}", type(g).__name__
    )

    print("dynamic posit tests passed!")


if __name__ == "__main__":
    print("SoftPosit Python Wrapper Tests")
    print("=" * 40)

    try:
        test_posit8()
        test_posit16()
        test_posit32()
        test_quire()
        test_dynamic_posit()

        print("\nAll tests passed!")
        sys.exit(0)
    except AssertionError as e:
        print("Test failed:", e)
        sys.exit(1)
