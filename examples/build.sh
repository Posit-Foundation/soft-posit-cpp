#!/bin/bash

# Exit on error
set -e

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Default install location (can be overridden)
INSTALL_DIR="${SCRIPT_DIR}/../install"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --install-dir=*)
            INSTALL_DIR="${key#*=}"
            shift
            ;;
        *)
            echo "Unknown option: $key"
            exit 1
            ;;
    esac
done

# Check if install directory exists
if [ ! -d "$INSTALL_DIR" ]; then
    echo "Error: Install directory not found: $INSTALL_DIR"
    echo "Please build and install SoftPosit library first or specify the install location:"
    echo "./build.sh --install-dir=/path/to/softposit/install"
    exit 1
fi

echo "Using SoftPosit from: $INSTALL_DIR"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_PREFIX_PATH="$INSTALL_DIR"

# Build
echo "Building examples..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "Build completed successfully!"
echo ""
echo "To run the Mandelbrot example:"
echo "  cd build/mandelbrot"
echo "  ./posit_mandelbrot_dynamic"
echo ""
echo "To run the Multiplication Closure example:"
echo "  cd build/multiplication_closure"
echo "  ./multiplication_closure_dynamic"
echo ""
echo "PPM image files will be generated in the current directory." 