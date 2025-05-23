# SoftPosit Python Wrapper Installation Guide

This guide provides instructions for installing the SoftPosit Python wrapper on different operating systems.

## Prerequisites

The SoftPosit Python wrapper requires:

- Python 3.6 or higher
- CMake 3.10 or higher
- SWIG 3.0 or higher
- A C/C++ compiler (GCC, Clang, MSVC, etc.)

Optional dependencies for examples:

- NumPy
- Matplotlib

## Installation Methods

### Method 1: Using pip directly from the source directory

This is the easiest method if you've already cloned the repository:

```bash
# Navigate to the python directory
cd python

# Install the package
pip install .

# For development mode (changes to C/C++ code will require rebuild)
pip install -e .
```

### Method 2: Using the build script

We provide a convenient build script that builds the library and runs tests:

```bash
# Make the script executable
chmod +x build_python_wrapper.sh

# Run the build script
./build_python_wrapper.sh
```

After running the script, you can use the Python wrapper by adding the python directory to your Python path:

```python
import sys
sys.path.append('/path/to/soft-posit-cpp/python')
import softposit
```

### Method 3: Building with CMake

You can also build the Python wrapper manually using CMake:

```bash
# Create a build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DBUILD_PYTHON_WRAPPER=ON

# Build
make

# Install (may require sudo)
make install
```

## Platform-specific Instructions

### Linux

On most Linux distributions, you can install the prerequisites with the package manager:

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install cmake swig python3-dev build-essential
pip install numpy matplotlib  # Optional, for examples
```

#### Fedora/RHEL/CentOS

```bash
sudo dnf install cmake swig python3-devel gcc-c++
pip install numpy matplotlib  # Optional, for examples
```

### macOS

On macOS, you can use Homebrew to install the prerequisites:

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install prerequisites
brew install cmake swig

# Install optional dependencies for examples
pip install numpy matplotlib
```

### Windows

On Windows, we recommend using conda to manage the prerequisites:

```bash
# Create a conda environment
conda create -n softposit python=3.8
conda activate softposit

# Install prerequisites
conda install -c conda-forge cmake swig
conda install -c conda-forge numpy matplotlib  # Optional, for examples
```

Then, build using either Method 1 or Method 3 above.

## Verifying the Installation

After installation, you can verify that the wrapper works correctly by running:

```bash
python -c "import softposit; print(softposit.posit8(1.5))"
```

If the installation was successful, this should print `1.5`.

You can also run the example script:

```bash
python softposit_example.py
```

And the test script:

```bash
python test_softposit.py
```

## Troubleshooting

### ImportError: No module named _softposit

This error indicates that the C extension module `_softposit.so` (or `.pyd` on Windows) is not in your Python path. Make sure:

1. The module was built successfully
2. The directory containing `_softposit.so` and `softposit.py` is in your Python path
3. The permissions on `_softposit.so` allow it to be read

### SWIG not found

If CMake cannot find SWIG, you may need to set the `SWIG_EXECUTABLE` variable:

```bash
cmake .. -DBUILD_PYTHON_WRAPPER=ON -DSWIG_EXECUTABLE=/path/to/swig
```

### Symbol not found: _PyFloat_FromDouble

This error can occur when the Python version used for building the module is different from the one being used to import it. Make sure you build and import with the same Python version.

## For Developers

If you're developing the SoftPosit library and need to rebuild the Python wrapper frequently, we recommend using the development mode installation:

```bash
pip install -e python/
```

This way, you only need to rebuild the C extension when you change the C/C++ code, not the Python code.
