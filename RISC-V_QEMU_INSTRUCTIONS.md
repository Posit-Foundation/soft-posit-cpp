# Running RISC-V Programs with QEMU

This guide explains how to compile and run RISC-V programs using QEMU emulation.

## Prerequisites

### Native Installation (Ubuntu/Debian)

```bash
# Update package list
sudo apt-get update

# Install QEMU user-mode emulation
sudo apt-get install -y qemu-user-static

# Install RISC-V cross-compilation toolchain
sudo apt-get install -y gcc-riscv64-linux-gnu g++-riscv64-linux-gnu

# Install RISC-V runtime libraries
sudo apt-get install -y libc6-dev-riscv64-cross
sudo apt-get install -y libstdc++6-riscv64-cross libgcc-s1-riscv64-cross

# Install build tools
sudo apt-get install -y cmake make ninja-build
```

### Using Docker (Recommended)

See the `Dockerfile.riscv` in this repository for a pre-configured environment.

```bash
# Build the Docker image
docker build -f Dockerfile.riscv -t soft-posit-riscv .

# Run interactive container
docker run -it --rm -v $(pwd):/workspace soft-posit-riscv
```

## Compilation

### Manual Compilation

```bash
# Create build directory
mkdir -p build-riscv
cd build-riscv

# Configure CMake for RISC-V cross-compilation
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
  -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"

# Build
make -j$(nproc)
```

### Using Build Script

```bash
# Make the script executable
chmod +x scripts/build_riscv.sh

# Run the build script
./scripts/build_riscv.sh
```

## Running Programs

### Basic Execution

```bash
# Navigate to build directory
cd build-riscv

# Make executables executable (if needed)
chmod +x *_tests

# Run individual test suites
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p8_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p16_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p32_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./q8_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./q16_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./q32_tests
```

### Running All Tests

```bash
# Using the convenience script
chmod +x scripts/run_riscv_tests.sh
./scripts/run_riscv_tests.sh
```

### Running Benchmarks

```bash
cd build-riscv
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./benchmarks/accuracy_benchmarks
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./benchmarks/performance_benchmarks
```

## QEMU Options Explained

- `qemu-riscv64-static`: QEMU static user-mode emulator for RISC-V 64-bit
- `-L /usr/riscv64-linux-gnu`: Specify the root filesystem path for libraries
- The static linking (`-static-libgcc -static-libstdc++`) reduces dependency issues

## Troubleshooting

### Common Issues

1. **"No such file or directory" when running executables**
   - Ensure QEMU is installed: `sudo apt-get install qemu-user-static`
   - Verify the executable was built for RISC-V: `file ./your_executable`

2. **Missing shared libraries**
   - Install RISC-V runtime libraries as shown in prerequisites
   - Ensure static linking is enabled in CMake configuration

3. **Build failures**
   - Check that cross-compilation toolchain is installed
   - Verify CMake finds the correct compilers: `which riscv64-linux-gnu-gcc`

### Verifying RISC-V Binary

```bash
# Check if binary is for RISC-V
file ./build-riscv/p8_tests
# Should output: ELF 64-bit LSB executable, UCB RISC-V, version 1 (SYSV)...

# Check dynamic dependencies (should be minimal with static linking)
ldd ./build-riscv/p8_tests
```

## Performance Considerations

- QEMU emulation will be slower than native execution
- Use `-cpu rv64` for specific CPU model if needed
- Consider using QEMU's JIT compilation for better performance

## Additional Resources

- [QEMU User Mode Documentation](https://qemu.readthedocs.io/en/latest/user/main.html)
- [RISC-V Cross Compilation Guide](https://risc-v-getting-started-guide.readthedocs.io/en/latest/linux-qemu.html)
- [CMake Cross Compilation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html) 