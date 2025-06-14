#!/bin/bash
set -e

echo "======================================"
echo "Building Soft Posit for RISC-V"
echo "======================================"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Check if RISC-V toolchain is available
if ! command -v riscv64-linux-gnu-gcc &> /dev/null; then
    echo "Error: RISC-V cross-compilation toolchain not found."
    echo "Please install it with:"
    echo "  sudo apt-get install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu"
    exit 1
fi

# Check if QEMU is available
if ! command -v qemu-riscv64-static &> /dev/null; then
    echo "Warning: QEMU not found. You won't be able to run the binaries directly."
    echo "Install with: sudo apt-get install qemu-user-static"
fi

# Create and enter build directory
echo "Creating build directory..."
mkdir -p build-riscv
cd build-riscv

# Configure CMake for RISC-V
echo "Configuring CMake for RISC-V cross-compilation..."
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DBUILD_BENCHMARKS=ON \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
  -DCMAKE_C_COMPILER=riscv64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=riscv64-linux-gnu-g++ \
  -DCMAKE_FIND_ROOT_PATH=/usr/riscv64-linux-gnu \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
  -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++" \
  -DREDUCE_P16_TESTS_ON_UBUNTU=ON

# Build
echo "Building..."
make -j$(nproc)

echo "======================================"
echo "Build completed successfully!"
echo "======================================"
echo "Binaries are located in: build-riscv/"
echo "To run tests: ./scripts/run_riscv_tests.sh"
echo "To run a specific binary: qemu-riscv64-static -L /usr/riscv64-linux-gnu ./build-riscv/<binary>"

# Verify binaries
echo ""
echo "Verifying built binaries:"
for binary in *_tests; do
    if [ -f "$binary" ]; then
        echo "  $(file "$binary")"
    fi
done 