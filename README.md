# <img src="https://posithub.org/images/NGA_logo.png"  width="90" height="50"/> SoftPosit

> This is a fork of the original [SoftPosit](https://gitlab.com/cerlane/SoftPosit) repository created by [Cerlane Leong](https://gitlab.com/cerlane). We would like to thank the original authors for their work.

[![Build Ubuntu](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/build-ubuntu.yml/badge.svg)](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/build-ubuntu.yml)
[![Build macOS](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/build-macos.yml/badge.svg)](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/build-macos.yml)
[![Run Tests](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/tests.yml/badge.svg)](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/tests.yml)
[![Code Quality](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/code-quality.yml/badge.svg)](https://github.com/Posit-Foundation/soft-posit-cpp/actions/workflows/code-quality.yml)

## Quick Reference

### Basic Types

```c
#include "softposit.h"

posit8_t  p8;   // 8-bit posit (0 exponent bits)
posit16_t p16;  // 16-bit posit (1 exponent bit)  
posit32_t p32;  // 32-bit posit (2 exponent bits)

quire8_t  q8;   // 8-bit quire (for accumulation)
quire16_t q16;  // 16-bit quire
quire32_t q32;  // 32-bit quire
```

### Common Operations

```c
// Arithmetic
posit16_t sum = p16_add(a, b);        // a + b
posit16_t diff = p16_sub(a, b);       // a - b  
posit16_t prod = p16_mul(a, b);       // a * b
posit16_t quot = p16_div(a, b);       // a / b
posit16_t fma = p16_mulAdd(a, b, c);  // a*b + c

// Conversions
posit16_t p = convertDoubleToP16(3.14159);
double d = convertP16ToDouble(p);
posit8_t p8 = p16_to_p8(p);

// Comparisons  
bool equal = p16_eq(a, b);     // a == b
bool less = p16_lt(a, b);      // a < b
bool less_eq = p16_le(a, b);   // a <= b

// Math functions (experimental)
posit16_t root = p16_sqrt(p);
posit16_t sine = p16_sin_pi(p);
posit16_t logarithm = p16_ln(p);
```

---

## Table of Contents

- [Quick Start](#quick-start)
- [Development Workflow](#development-workflow)
- [Advanced Build Options](#advanced-build-options)
- [Experimental Features](#experimental-features)
- [Project Structure](#project-structure)
- [Compatibility and Testing](#compatibility-and-testing)
- [Quick Reference](#quick-reference)
- [C API Documentation](#cversion)
- [C++ API Documentation](#cppversion)
- [Python Wrapper](#softposit-python-wrapper)
- [Julia Integration](#jversion)
- [Known Implementations](#known)

---

## Quick Start

### Prerequisites

- **CMake** 3.1 or higher
- **C/C++ Compiler**: GCC, Clang, or MSVC
- **clang-format-14** (for code formatting)

### Building and Testing

```bash
# Clone the repository
git clone https://github.com/Posit-Foundation/soft-posit-cpp.git
cd soft-posit-cpp

# Create build directory and configure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_BENCHMARKS=ON

# Build the library
make -j$(nproc)  # Linux
# make -j$(sysctl -n hw.ncpu)  # macOS

# Run all tests
make check

# Run all benchmarks
make bench
```

## Development Workflow

### Supported Branches

Our CI/CD workflows run on:

- **`main`** - Stable release branch
- **`refactor-cpp-soft-posits`** - Active development branch

Pull requests targeting either of these branches will trigger automated builds, tests, and code quality checks.

### Code Formatting

This project uses **clang-format-14** for consistent code formatting. Before submitting any changes:

1. **Format your code**:

   ```bash
   # Format all C/C++ files
   ./format_code.sh
   
   # Or manually with clang-format-14
   find . -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | \
     xargs clang-format-14 -i
   ```

2. **Verify formatting**:

   ```bash
   clang-format-14 --dry-run --Werror src/ include/ tests/ examples/
   ```

The formatting configuration is defined in [`.clang-format`](.clang-format).

### Continuous Integration

Our automated workflows include:

- **Build Validation**: Cross-platform builds (Ubuntu, macOS)
- **Comprehensive Testing**: All posit operations and edge cases
- **Code Quality Checks**:
  - clang-format style enforcement
- **Performance Benchmarks**: Automated performance regression detection

### Contributing

1. **Fork the repository** and create a feature branch from `refactor-cpp-soft-posits`
2. **Make your changes** following the coding standards
3. **Format your code** using `./format_code.sh`
4. **Run tests locally**: `cd build && make check`
5. **Submit a pull request** targeting `refactor-cpp-soft-posits` or `main`

All pull requests must pass:

- ✅ Build validation on Ubuntu and macOS
- ✅ Complete test suite
- ✅ Code formatting checks (clang-format-14)

## Advanced Build Options

### Build Configuration

```bash
# Development build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_BENCHMARKS=ON

# Build with experimental features
cmake .. -DENABLE_EXPERIMENTAL_TESTS=ON

# Custom installation directory
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | `ON` | Build test executables |
| `BUILD_BENCHMARKS` | `OFF` | Build benchmark executables |
| `ENABLE_EXPERIMENTAL_TESTS` | `OFF` | Enable tests for experimental features |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries instead of static |

### Installation

```bash
# Install the library and headers
cd build
make install

# Create distribution packages
make package
```

## Legacy Build Information

> **Note**: The legacy Makefile-based build system is deprecated. Please use CMake as shown above.

## Experimental Features

Some advanced mathematical functions are under active development and can be enabled with the `ENABLE_EXPERIMENTAL_TESTS` option:

```bash
cmake .. -DENABLE_EXPERIMENTAL_TESTS=ON
```

**Experimental functions include:**

- **Trigonometric**: `sin_pi`, `cos_pi`, `tan_pi`
- **Inverse trigonometric**: `asin_pi`, `acos_pi`, `atan_pi`
- **Logarithmic**: `log2`, `ln`
- **Exponential**: `exp`, `exp2`
- **Rounding**: `ceil`, `floor`

> ⚠️ **Warning**: Experimental functions may not be fully tested or optimized. Use with caution in production code.

This version (0.4.1) supports:

    32-bit with two exponent bit (posit32_t).  -> Not exhaustively tested

    16-bit with one exponent bit (posit16_t). 
    
    8-bit with zero exponent bit (posit8_t). 
    
    2-bit to 32-bit with two exponent bits (posit_2_t) -> Not fast : Using 32-bits in the background to store all sizes.
        Exhaustively tested for X=(2:32) : pX2_rint, pX2_to_pX2, pX2_to_i32/64, pX2_to_ui32/64, pX2_sqrt, ui/i32_to_pX2
        Exhaustively tested for X=(2:13) : ui64_to_pX2, i64_to_pX2
        Exhaustively tested for X=(2:20) : pX2_add, pX2_sub, pX2_mul, pX2_div
        Exhaustively tested for X=(2:21) : pX2_mul
        Exhaustively tested for X=(2:15) : pX2_mulAdd
        Exhaustively tested for X=(2:14) : quireX2_fdp_add, quireX2_fdp_sub (using quire32 as the underlying code)
    
    

This code is tested on

- GNU gcc (SUSE Linux) 4.8.5
- Apple LLVM version 9.1.0 (clang-902.0.39.2)
- Windows 10 (Mingw-w64)

Please note that the same Makefile in build/Linux-x86_64-GCC is used for all 3 operating systems.

 All posit8_t and posit16_t operations are exhaustively tested with exception of p16_mulAdd and q16_fdp_add/sub operations.

 **posit32_t operations are still being tested exhaustively for correctness. It will take weeks to months before these tests complete.**

 Versions are offered

- [Fast C version](#cversion) : The main source code where all other versions are based on.
- [User friendly C++ version](#cppversion)  : Documentation can be found below.
- [User friendly Python version](https://gitlab.com/cerlane/SoftPosit-Python/) : <https://gitlab.com/cerlane/SoftPosit-Python/>
- [Julia](#jversion)  : Currently only simple .so support. Documentation can be found below.
- [Others](#known)

## <a name="cversion"/>Fast C version

### Examples

#### A 8-bit example on how to use the code to add

```
#include "softposit.h"

int main (int argc, char *argv[]){

    posit8_t pA, pB, pZ;
    pA = castP8(0xF2);
    pB = castP8(0x23);

    pZ = p8_add(pA, pB);

    //To check answer by converting it to double
    double dZ = convertP8ToDouble(pZ);
    printf("dZ: %.15f\n", dZ);

    //To print result in binary
    uint8_t uiZ = castUI(pZ);
    printBinary((uint64_t*)&uiZ, 8);
    
    return 0;

}
```

#### A 16-bit example on how to use the code to multiply

```
#include "softposit.h"

int main (int argc, char *argv[]){

    posit16_t pA, pB, pZ;
    pA = castP16(0x0FF2);
    pB = castP16(0x2123);

    pZ = p16_mul(pA, pB);

   //To check answer by converting it to double
    double dZ = convertP16ToDouble(pZ);
    printf("dZ: %.15f\n", dZ);

    //To print result in binary
    uint16_t uiZ = castUI(pZ);
    printBinary((uint64_t*)&uiZ, 16);

    return 0;
}
```

#### A 24-bit (es=2) example on how to use the code

```
#include "softposit.h"

int main (int argc, char *argv[]){

    posit_2_t pA, pB, pZ;
    pA.v = 0xF2; //this is to set the bits (method 1)
    pB = castPX2(0x23); //this is to set the bits (method 2)

    pZ = pX2_add(pA, pB, 24);

    //To check answer by converting it to double
    double dZ = convertPX2ToDouble(pZ);
    printf("dZ: %.40f\n", dZ);

    //To print result in binary
    printBinaryPX((uint32_t*)&pZ.v, 24);
    
    //To print result as double
    printf("result: %.40f\n", convertPX2ToDouble(pZ));
    
    return 0;

}
```

#### For deep learning, please use quire

```
//Convert double to posit
posit16_t pA = convertDoubleToP16(1.02783203125 );
posit16_t pB = convertDoubleToP16(0.987060546875);
posit16_t pC = convertDoubleToP16(0.4998779296875);
posit16_t pD = convertDoubleToP16(0.8797607421875);

quire16_t qZ;

//Set quire to 0
qZ = q16_clr(qZ);

//accumulate products without roundings
qZ = q16_fdp_add(qZ, pA, pB);
qZ = q16_fdp_add(qZ, pC, pD);

//Convert back to posit
posit16_t pZ = q16_to_p16(qZ);

//To check answer
double dZ = convertP16ToDouble(pZ);
```

### Build and link

#### Build - softposit.a

Please note that only 64-bit systems are supported. For Mac OSX and Linux, the same Makefile is used.

Note that architecture specific optimisation is removed. To get maximum speed, please update OPTIMISATION flag in build/Linux-x86_64-GCC/Makefile.

```
cd SoftPosit/build/Linux-x86_64-GCC
make -j6 all

```

#### Link - softposit.a

If your source code is for example "main.c" and you want to create an executable "main".
Assume that SoftPosit is installed and installed in the same directory (installing in the same directory is NOT recommended).

```
gcc -lm -o main \
    main.c SoftPosit/build/Linux-x86_64-GCC/softposit.a  -ISoftPosit/source/include -O2 

```

### Features

#### Main Posit Functionalities

Add :

     posit16_t p16_add(posit16_t, posit16_t)
     
     posit8_t p8_add(posit8_t, posit8_t)

Subtract :

    posit16_t p16_sub(posit16_t, posit16_t)
    
    posit8_t p8_sub(posit8_t, posit8_t)
    

Divide :

    posit16_t p16_div(posit16_t, posit16_t)
    
    posit8_t p8_div(posit8_t, posit8_t)

Multiply :

    posit16_t p16_mul(posit16_t, posit16_t)
    
    posit8_t p8_mul(posit8_t, posit8_t)
    

Fused Multiply Add :

    posit16_t p16_mulAdd(posit16_t, posit16_t, posit16_t)
    
    posit8_t p8_mulAdd(posit8_t, posit8_t, posit8_t)
    
    
    Note: p16_mulAdd(a, b, c) <=> a*b + c

#### Main Quire Functionalities

Fused dot product-add  :

    quire16_t q16_fdp_add(quire16_t, posit16_t, posit16_t)
    
    quire8_t q16_fdp_add(quire8_t, posit8_t, posit8_t)
    
    Note: q8_fdp_add (a, b, c) <=> a + b*c

Fused dot product-subtract  :

    quire16_t q16_fdp_sub(quire16_t, posit16_t, posit16_t)
    
    quire8_t q8_fdp_sub(quire8_t, posit8_t, posit8_t)

Set quire variable to zero :

    quire16_t q16_clr(quire16_t)
    
    quire8_t q8_clr(quire8_t)

Convert quire to posit :

    posit16_t q16_to_p16(quire16_t)
    
    posit8_t q8_to_p8(quire8_t)

#### Functionalites in Posit Standard

Square root :

    posit16_t p16_sqrt(posit16_t)
    
    posit8_t p8_sqrt(posit8_t)

Round to nearest integer :

    posit16_t p16_roundToInt(posit16_t)
    
    posit8_t p8_roundToInt(posit8_t)

Check equal :

    bool p16_eq( posit16_t, posit16_t )
    
    bool p8_eq( posit8_t, posit8_t )

Check less than equal :

    bool p16_le( posit16_t, posit16_t )
    
    bool p8_le( posit8_t, posit8_t )

Check less than :

    bool p16_lt( posit16_t, posit16_t )
    
    bool p8_lt( posit8_t, posit8_t )

Convert posit to integer (32 bits) :

    int_fast32_t p16_to_i32( posit16_t )
    
    int_fast32_t p8_to_i32( posit8_t )

Convert posit to long long integer (64 bits) :

    int_fast64_t p16_to_i64( posit16_t)
    
    int_fast64_t p8_to_i64( posit8_t)

Convert unsigned integer (32 bits) to posit:

    posit16_t ui32_to_p16( uint32_t a )
    
    posit8_t ui32_to_p8( uint32_t a )

Convert unsigned long long int (64 bits) to posit:

    posit16_t ui64_to_p16( uint64_t a )
    
    posit8_t ui64_to_p8( uint64_t a )

Convert integer (32 bits) to posit:

    posit16_t i32_to_p16( int32_t a )
    
    posit8_t i32_to_p8( uint32_t a )

Convert long integer (64 bits) to posit:

    posit16_t i64_to_p16( int64_t a )
    
    posit8_t i64_to_p8( uint64_t a )

Convert posit to unsigned integer (32 bits) :

    uint_fast32_t p16_to_ui32( posit16_t )
    
    uint_fast32_t p8_to_ui32( posit8_t )

Convert posit to unsigned long long integer (64 bits) :

    uint_fast64_t p16_to_ui64( posit16_t)
    
    uint_fast64_t p8_to_ui64( posit8_t)
    
Convert posit to integer (32 bits) :

    uint_fast32_t p16_to_i32( posit16_t )
    
    uint_fast32_t p8_to_i32( posit8_t )

Convert posit to long long integer (64 bits) :

    uint_fast64_t p16_to_i64( posit16_t)
    
    uint_fast64_t p8_to_i64( posit8_t)

Convert posit to posit of another size :

    posit8_t p16_to_p8( posit16_t )
    
    posit32_t p16_to_p32( posit16_t )
    
    posit16_t p8_to_p16( posit8_t )
    
    posit32_t p8_to_p32( posit8_t )

#### Helper Functionalites (NOT in Posit Standard)

Convert posit to double (64 bits) :

    double convertP16ToDouble(posit16_t)
    
    double convertP8ToDouble(posit8_t)

Convert double (64 bits) to posit  :

    posit16_t convertDoubleToP16(double)
    
    posit8_t convertDoubleToP8(double)
    
Cast binary expressed in unsigned integer to posit :

    posit16_t castP16(uint16_t)
    
    posit8_t castP8(uint8_t)
    
    uint16_t castUI(posit16_t)
    
    uint8_t castUI(posit8_t)
    

## <a name="cppversion"/>Easy to use C++ version

### Build and Link

**Build and link your C++ program to SoftPosit.a (C)**

Please compile your executable with g++ and not gcc.

```
g++ -std=gnu++11 -o main \
 ../source/testmain.cpp \
 ../../SoftPosit/source/../build/Linux-x86_64-GCC/softposit.a  \
 -I../../SoftPosit/source/../build/Linux-x86_64-GCC  -O2
```

### Example

#### Example of testmain.cpp

```
#include "softposit_cpp.h"

int main(int argc, char *argv[]){
 posit16 x = 1;
 posit16 y = 1.5;
 posit8 x8 = 1;
 quire16 q;
 quire8 q8;

 x += p16(1.5)*5.1;

 printf("%.13f  sizeof: %d\n", x.toDouble(), sizeof(posit16));

 x = q.qma(4, 1.2).toPosit();
 printf("%.13f  sizeof: %d\n", x.toDouble(), sizeof(quire16));

 x8 = q8.qma(4, 1.2).toPosit();
 printf("%.13f  sizeof: %d\n", x8.toDouble(), sizeof(quire8));
 
 std::cout << x;

 return 0;
}


```

### Functionalities

#### Main functionalities

- Posit types: posit16, posit8
- Fused-multiply-add:
  - posit16 fma(posit16, posit16, posit16)
  - posit18 fma(posit18, posit18, posit8)
- Square root:
  - posit16 sqrt(posit16)
  - posit8 sqrt(posit8)
- roundToInt:
  - posit16 rint(posit16)
  - posit8 rint(posit8)
- Supported operators
  - \+
  - +=
  - \-
  - \-=
  - &ast;
  - &ast;=
  - /
  - /=
  - <<
  - <<=
  - &#62;&#62;
  - &#62;&#62;=
  - &
  - &=
  - |
  - |=
  - ^
  - ^=
  - &&
  - ||
  - ++
  - --
  - ==
  - ~
  - !
  - !=
  - &ast;
  - <
  - &ast;=
  - <=
- Posit to Double:
  - double (instance of posit).toDouble()
- Double to Posit:
  - posit16 p16(double)
  - posit8 p8(double)
- Posit to NaR:
  - posit16 (instance of posit16).toNaR()
  - posit8 (instance of posit8).toNaR()

#### Quire functionalities (particularly for deep learning)

- Quire types: quire16, quire8 (when declared, quire is initiated to zero)
- Clear quire to zero:
  - (instance of quire16).clr()
- Quire multiply add (fused)
  - (instance of quire16).fma(quire16)
  - (instance of quire8).fma(quire8)
- Quire multiply subtract (fused)
  - (instance of quire16).fms(quire16)
  - (instance of quire8).fms(quire8)
- Convert quire to Posit
  - posit16 (instance of quire16).toPosit()
  - posit8 (instance of quire8).toPosit()
- Check if quire is NaR
  - bool (instance of quire).isNaR()

## <a name="jversion"/>Julia

- [Julia implementation] (<https://github.com/milankl/SoftPosit.jl>) on top of SoftPosit

### Install via Julia package manager

```
> add https://github.com/milankl/SoftPosit.jl

```

Credits to Milan Klöwer.

### Behind the scene

#### Build shared library

```
cd SoftPosit/build/Linux_x86_64_GCC/
make -j6 julia
```

#### Simple Tests

```
julia> t = ccall((:convertDoubleToP16, "/path/to/SoftPosit/build/Linux-x86_64-GCC/softposit.so"), UInt16, (Float64,),1.0)
0x4000

julia> t = ccall((:convertDoubleToP16, "/path/to/SoftPosit/build/Linux-x86_64-GCC/softposit.so"), UInt16, (Float64,),-1.0)
0xc000

```

## <a href="known"/>Known implementations on top of SoftPosit

- [Andrey Zgarbul's Rust implementation](https://crates.io/crates/softposit)
- [Milan Klöwer's Julia implementation](https://github.com/milankl/SoftPosit.jl)
- [SpeedGo Computing's TensorFlow](https://github.com/xman/tensorflow/tree/posit)
- [SpeedGo Computing's Numpy](https://github.com/xman/numpy-posit)
- [Cerlane Leong's SoftPosit-Python](https://gitlab.com/cerlane/SoftPosit-Python)
- [David Thien's SoftPosit bindings Racket](https://github.com/DavidThien/softposit-rkt)
- [Bill Zorn's SoftPosit and SoftFloat Python](https://pypi.org/project/sfpy/)

# SoftPosit Python Wrapper

This package provides Python bindings for the [SoftPosit](https://github.com/cjdelisle/SoftPosit) library, which implements the posit number format.

## What are Posits?

Posits are a new number format proposed by John L. Gustafson as an alternative to IEEE 754 floating-point numbers. They offer several advantages:

- Higher accuracy with fewer bits
- Graceful underflow to zero
- No separate handling for infinities, NaN, or subnormals
- Consistent behavior across all bit widths
- Faster and simpler hardware implementation

## Installation

### Prerequisites

- CMake (version 3.1 or higher)
- C++ compiler (GCC, Clang, MSVC)
- Python 3.6+

### Installation from Source

1. Clone the repository:

   ```bash
   git clone https://github.com/cjdelisle/SoftPosit.git
   cd SoftPosit/python
   ```

2. Install the package:

   ```bash
   # Option 1: Development installation
   pip install -e .
   
   # Option 2: Regular installation
   pip install .
   ```

3. Verify the installation:

   ```python
   import softposit
   p8 = softposit.posit8(3.5)
   print(p8)  # Should print 3.5
   ```

## Usage

The wrapper provides three main posit types:

- `posit8`: 8-bit posit numbers
- `posit16`: 16-bit posit numbers
- `posit32`: 32-bit posit numbers

### Basic Operations

```python
import softposit

# Create posit numbers
p8 = softposit.posit8(3.14)
p16 = softposit.posit16(2.718)
p32 = softposit.posit32(1.414)

# Arithmetic operations
sum_p8 = p8 + 1.5        # Adding float
diff_p16 = p16 - p16/2   # Subtraction
prod_p32 = p32 * 2       # Multiplication
div_p8 = p8 / 2          # Division

# Conversion between types
p16_from_p8 = softposit.posit16(p8)
p32_from_p16 = softposit.posit32(p16)

# Conversion to Python types
float_val = float(p8)
int_val = int(p32)

# Print values
print(f"p8 = {p8}")
print(f"p16 = {p16}")
print(f"p32 = {p32}")
print(f"p8 + 1.5 = {sum_p8}")
print(f"p16 - p16/2 = {diff_p16}")
print(f"p32 * 2 = {prod_p32}")
print(f"p8 / 2 = {div_p8}")
```

### Numerical Properties

Posit numbers have different precision characteristics than floats. Here's an example showing
the precision differences:

```python
import softposit

# Original value (exact in decimal)
value = 1/3  # 0.3333...

# Create posit representations
p8 = softposit.posit8(value)
p16 = softposit.posit16(value)
p32 = softposit.posit32(value)

# Print values and errors
print(f"Original value: {value}")
print(f"posit8:  {p8}  (error: {abs(float(p8) - value)})")
print(f"posit16: {p16} (error: {abs(float(p16) - value)})")
print(f"posit32: {p32} (error: {abs(float(p32) - value)})")
```

## Examples

The package includes several example files:

- `basic_test.py`: Basic functionality test
- `softposit_example.py`: Comprehensive usage examples
- `test_softposit.py`: More extensive tests

Run these examples to explore the capabilities of the posit number format:

```bash
python softposit_example.py
```

## Troubleshooting

### ImportError: No module named '_softposit'

If you see this error, it means the C extension module was not properly installed. Try:

1. Reinstalling the package with `pip install -e .`
2. Check that your Python version matches the one used during compilation
3. Make sure the shared library `libsoftposit.so` (or `libsoftposit.dylib` on macOS) is in your library path

### Performance Considerations

Posit operations in this Python wrapper are implemented in C/C++ for performance, but there's still overhead from Python bindings. For maximum performance in numerical code, consider using arrays of posits and native C++ code.

## License

This package is released under the same license as the original SoftPosit library.

## Project Structure

```
soft-posit-cpp/
├── src/                    # Core C implementation
├── include/                # Public header files
├── tests/                  # Comprehensive test suites
│   ├── p8_tests/          # posit8 tests
│   ├── p16_tests/         # posit16 tests
│   ├── p32_tests/         # posit32 tests
│   └── common_tests/      # Shared utilities
├── examples/              # Usage examples and demos
├── benchmarks/            # Performance benchmarks
├── python/                # Python wrapper (optional)
├── .github/workflows/     # CI/CD pipeline definitions
└── cmake/                 # CMake modules and configuration
```

## Compatibility and Testing

### Platform Support

- **Linux**: Ubuntu 20.04+, tested with GCC 9+ and Clang 10+
- **macOS**: macOS 11+, tested with Apple Clang 12+
- **Windows**: Experimental support (contributions welcome)

### Test Coverage

- ✅ **posit8**: Exhaustively tested (all operations)
- ✅ **posit16**: Extensively tested (most operations)
- 🔄 **posit32**: Comprehensive testing in progress
- ✅ **Quire operations**: Fully tested for posit8/posit16
- ✅ **Type conversions**: All combinations tested

Our automated test suite runs ~1M+ test cases across different posit sizes and operations.
