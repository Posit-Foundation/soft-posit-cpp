# SoftPosit Python Wrapper

This is a Python wrapper for the SoftPosit library, which implements the Posit number system.

## What are Posits?

Posits are a new number format that can replace floating-point numbers. They offer higher accuracy, larger dynamic range, better closure, and simpler hardware than IEEE Standard 754 floating-point, in the same number of bits.

For more information on Posits, please visit [The Posit Working Group](https://posithub.org/).

## Installation

### Prerequisites

To build the SoftPosit Python wrapper, you need:

- CMake (version 3.10 or higher)
- SWIG (version 3.0 or higher)
- Python development headers
- A C/C++ compiler (gcc, clang, etc.)

#### On Ubuntu/Debian

```bash
sudo apt-get install cmake swig python3-dev build-essential
```

#### On macOS

```bash
brew install cmake swig
```

### Building the wrapper

1. Clone the SoftPosit repository:

   ```bash
   git clone https://github.com/yourusername/soft-posit-cpp.git
   cd soft-posit-cpp
   ```

2. Build the library and Python wrapper:

   ```bash
   ./build_python_wrapper.sh
   ```

3. Test the installation:

   ```bash
   cd python
   python3 softposit_example.py
   ```

## Usage

### Basic operations

```python
import softposit

# Create posit values
p8 = softposit.posit8(1.5)    # 8-bit posit
p16 = softposit.posit16(2.0)  # 16-bit posit
p32 = softposit.posit32(3.75) # 32-bit posit

# Basic arithmetic operations
sum_result = p8 + p16         # Addition (automatically converts to appropriate precision)
diff_result = p16 - p8        # Subtraction
prod_result = p8 * p16        # Multiplication
div_result = p32 / p16        # Division

# Convert posits to Python types
float_value = float(p8)       # Convert to float
int_value = int(p8)           # Convert to int (truncates)

# Create posits from other posits
p8_from_p32 = softposit.posit8(float(p32))  # Convert from higher to lower precision
```

### Advanced operations

```python
# Creating posits from bit patterns
p8_bits = softposit.posit8(bits=0x50)  # Create from bit pattern

# Math operations
p16_sqrt = softposit.posit16(4.0)**0.5  # Square root

```

## Available Posit Types

The wrapper supports the following posit formats:

- `posit8`: 8-bit posit values
- `posit16`: 16-bit posit values
- `posit32`: 32-bit posit values

## Examples

See the `softposit_example.py` script for comprehensive examples of how to use the SoftPosit Python wrapper.

## Contributors

- Original SoftPosit C implementation: S.H. Leong (Cerlane)
- Python wrapper: SoftPosit contributors
