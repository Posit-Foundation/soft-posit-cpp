#!/bin/bash
set -e

# Function to print error message and exit
error_exit() {
    echo "ERROR: $1" >&2
    exit 1
}

# Function to print information
info() {
    echo "INFO: $1"
}

# Build the SoftPosit library and Python wrapper
info "Building SoftPosit library and Python wrapper..."

# Detect the operating system
OS=$(uname -s)
info "Detected OS: $OS"

# Print Python version
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    info "Using Python: $PYTHON_VERSION"
else
    error_exit "Python3 is not installed. Please install Python3 and try again."
fi

# Check if SWIG is installed
if command -v swig &> /dev/null; then
    SWIG_VERSION=$(swig -version | grep Version | awk '{print $3}')
    info "Using SWIG version: $SWIG_VERSION"
else
    error_exit "SWIG is not installed. Please install SWIG and try again."
fi

# Clean any previous build artifacts
info "Cleaning previous build artifacts..."
rm -rf python/_softposit.so python/_softposit*.so python/softposit_python_wrap.c python/*.pyc

# Get current directory (to properly set library paths)
CURRENT_DIR=$(pwd)
info "Current directory: $CURRENT_DIR"

# Check if build directory exists, if not create it
if [ ! -d "build" ]; then
    mkdir build
    info "Created build directory"
fi

# Skip CMake if the user just wants to rebuild the Python wrapper
if [ "$1" != "--skip-cmake" ]; then
    # Configure and build
    cd build
    info "Running CMake configuration..."
    cmake .. -DBUILD_PYTHON_WRAPPER=OFF -DBUILD_PYTHON3_WRAPPER=OFF -DBUILD_SHARED_LIBS=ON || error_exit "CMake configuration failed"

    # Determine the number of CPU cores for parallel build
    if [ "$OS" = "Darwin" ]; then
        # macOS (doesn't have nproc)
        CORES=$(sysctl -n hw.ncpu)
    else
        # Linux and others
        CORES=$(nproc 2>/dev/null || echo "4")
    fi
    
    info "Building SoftPosit library with $CORES cores..."
    make -j"$CORES" || error_exit "Build failed"
    cd ..
else
    info "Skipping CMake and build (--skip-cmake option used)"
    cd build && cd ..
fi

# Copy the shared library to the python directory to make imports easier
if [ "$OS" = "Darwin" ]; then
    info "Copying dylib to python directory for macOS..."
    cp build/libsoftposit.dylib python/ || error_exit "Failed to copy dylib"
else
    info "Copying so to python directory for Linux..."
    cp build/libsoftposit.so python/ || error_exit "Failed to copy so"
fi

# Use the simplified SWIG interface file 
info "Running SWIG manually to generate wrapper using simplified interface..."
swig -python -o python/softposit_python_wrap.c python/simplified_softposit.i || error_exit "SWIG failed to generate wrapper"

info "Compiling Python module..."
if [ "$OS" = "Darwin" ]; then
    # macOS
    # Get various Python-specific flags and paths
    info "Getting Python configuration on macOS..."
    LDSHARED_CMD=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LDSHARED'))" 2>/dev/null) || error_exit "Failed to get LDSHARED from Python"
    EXT_SUFFIX=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX') or '.so')" 2>/dev/null) || error_exit "Failed to get EXT_SUFFIX from Python"
    
    # If EXT_SUFFIX is empty, use default .so
    if [ -z "$EXT_SUFFIX" ]; then
        EXT_SUFFIX=".so"
    fi
    
    info "Extension suffix: $EXT_SUFFIX"
    
    # Compile the wrapper
    CC=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('CC'))" 2>/dev/null) || CC=clang
    CFLAGS=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('CFLAGS'))" 2>/dev/null)
    PYTHON_INCLUDE=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('INCLUDEPY'))" 2>/dev/null) || error_exit "Failed to get Python include path"
    
    info "Compiling wrapper with $CC..."
    $CC $CFLAGS -c python/softposit_python_wrap.c -o python/softposit_python_wrap.o -fPIC -I./include -I$PYTHON_INCLUDE || error_exit "Failed to compile wrapper"
    
    # Link the module with rpath set to the current directory
    info "Linking module..."
    $LDSHARED_CMD python/softposit_python_wrap.o -o python/_softposit$EXT_SUFFIX -L./python -lsoftposit -Wl,-rpath,@loader_path/. || error_exit "Failed to link module"
    
    # Optional: install using system paths
    info "Module compiled successfully to python/_softposit$EXT_SUFFIX"
else
    # Linux and others
    info "Compiling Python module on Linux..."
    python3 -m compileall -f python/softposit.py || error_exit "Failed to compile softposit.py"
    
    # Get Python include directory
    PYTHON_INCLUDE=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('INCLUDEPY'))" 2>/dev/null) || error_exit "Failed to get Python include path"
    info "Python include path: $PYTHON_INCLUDE"
    
    # Compile the wrapper
    info "Compiling wrapper..."
    gcc -fPIC -c python/softposit_python_wrap.c -o python/softposit_python_wrap.o -I./include -I$PYTHON_INCLUDE || error_exit "Failed to compile wrapper"
    
    # Link the module with rpath set to current directory
    info "Linking module..."
    gcc -shared python/softposit_python_wrap.o -o python/_softposit.so -L./python -lsoftposit -Wl,-rpath,'$ORIGIN' || error_exit "Failed to link module"
    
    info "Module compiled successfully to python/_softposit.so"
fi

# Make sure the softposit.py and _softposit.so files are in the python directory
info "Checking if Python wrapper files exist..."
# Check for both .so and platform-specific extension suffix
if [ ! -f "python/_softposit.so" ] && [ ! -f "python/_softposit$EXT_SUFFIX" ]; then
    error_exit "Python wrapper library (_softposit.so or _softposit$EXT_SUFFIX) not found."
fi

if [ ! -f "python/softposit.py" ]; then
    error_exit "Python wrapper module (softposit.py) not found."
fi

# Minimal Python wrapper to test basic functionality
info "Creating a basic test script..."
cat > python/basic_test.py << 'EOF'
#!/usr/bin/env python
# Basic test for SoftPosit Python wrapper

import sys
import os

# Try to import the module
try:
    import _softposit
    import softposit
    print("Successfully imported softposit modules")
except ImportError as e:
    print(f"Import error: {e}")
    sys.exit(1)

# Try to create a posit8 value
try:
    p8 = softposit.posit8(1.5)
    print(f"Created posit8(1.5) = {p8}")
    sys.exit(0)
except Exception as e:
    print(f"Error creating posit8: {e}")
    sys.exit(1)
EOF

# Run the basic test script
info "Running basic test..."
cd python

# Set library path for dynamic loading
if [ "$OS" = "Darwin" ]; then
    # For macOS
    export DYLD_LIBRARY_PATH="$CURRENT_DIR/python:$DYLD_LIBRARY_PATH"
else
    # For Linux
    export LD_LIBRARY_PATH="$CURRENT_DIR/python:$LD_LIBRARY_PATH"
fi

python3 basic_test.py || error_exit "Basic test failed"

info "Basic test passed! The SoftPosit Python wrapper is working." 