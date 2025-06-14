# Quick Start: RISC-V with QEMU

This guide shows you how to quickly get started with running RISC-V programs using QEMU.

## Quick Commands

```bash
# Docker approach (recommended)
docker build -f Dockerfile.riscv -t soft-posit-riscv .
docker run -it --rm -v $(pwd):/workspace soft-posit-riscv
build-riscv.sh && run-riscv-tests.sh

# Individual binary execution
run-riscv.sh build-riscv/p8_tests --gtest_filter="Posit8Special.*"
```

## Option 1: Using Docker (Recommended)

The easiest way to get started is using the pre-configured Docker environment:

```bash
# Build the Docker image
docker build -f Dockerfile.riscv -t soft-posit-riscv .

# Run the container with your code mounted
docker run -it --rm -v $(pwd):/workspace soft-posit-riscv

# Inside the container, build and run tests
build-riscv.sh
run-riscv-tests.sh
```

## Option 2: Native Installation

### Install Dependencies (Ubuntu/Debian)

```bash
# Install QEMU and RISC-V toolchain
sudo apt-get update
sudo apt-get install -y qemu-user-static gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
sudo apt-get install -y libc6-dev-riscv64-cross libstdc++6-riscv64-cross libgcc-s1-riscv64-cross
```

### Build and Run

```bash
# Build for RISC-V
./scripts/build_riscv.sh

# Run all tests
./scripts/run_riscv_tests.sh

# Or run individual tests
cd build-riscv
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p8_tests
```

## Verification

Check that everything is working:

```bash
# Verify QEMU is installed
qemu-riscv64-static --version

# Verify cross-compiler
riscv64-linux-gnu-gcc --version

# Check if binary is RISC-V
file ./build-riscv/p8_tests
# Should show: ELF 64-bit LSB executable, UCB RISC-V...
```

## Troubleshooting

### "No such file or directory" when running binaries
- Make sure QEMU is installed: `sudo apt-get install qemu-user-static`
- The error might be misleading - it usually means QEMU can't find the interpreter

### Shared library errors
- Make sure you have the RISC-V runtime libraries installed
- Our build uses static linking to avoid most dependency issues

### Build failures
- Ensure the cross-compiler is installed: `which riscv64-linux-gnu-gcc`
- Check that cmake found the right compiler in the build output

## What's Happening Under the Hood

1. **Cross-compilation**: We compile the code on x86_64 but target RISC-V architecture
2. **Static linking**: We link standard libraries statically to avoid runtime dependencies
3. **QEMU emulation**: QEMU translates RISC-V instructions to run on your host CPU
4. **Root filesystem**: The `-L /usr/riscv64-linux-gnu` tells QEMU where to find system libraries

For more detailed information, see `RISC-V_QEMU_INSTRUCTIONS.md`. 