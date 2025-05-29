# GitHub Actions Workflows

This guide explains the GitHub Actions workflows for the soft-posit-cpp project.

## Workflow Structure

We've organized our workflows for better clarity and efficiency:

1. **Build Workflows**:
   - `build-ubuntu.yml`: Builds the project on Ubuntu
   - `build-macos.yml`: Builds the project on macOS
   - `build-riscv.yml`: Cross-compiles and tests the project for RISC-V architecture

2. **Testing & Benchmarking**:
   - `tests.yml`: Runs tests on cached builds (supports Ubuntu, macOS, and RISC-V)
   - `benchmarks.yml`: Runs benchmarks on Ubuntu and RISC-V with cached builds

3. **Code Quality**:
   - `code-quality.yml`: Runs clang-format and clang-tidy checks

4. **Release**:
   - `release.yml`: Creates packages and releases for tagged versions

5. **Development**:
   - `local-dev.yml`: Simplified workflow for local development and testing

## RISC-V Cross-Compilation Support

The project now includes comprehensive RISC-V support through cross-compilation:

### RISC-V Build Process
- Uses GCC RISC-V cross-compilation toolchain (`gcc-riscv64-linux-gnu`)
- Builds for 64-bit RISC-V architecture
- Creates statically linked binaries for better portability
- Includes build verification to ensure proper RISC-V binary generation

### RISC-V Testing
- Tests are executed using QEMU user-mode emulation (`qemu-riscv64-static`)
- Full test suite runs including all posit and quire tests (p8, p16, p32, q8, q16, q32)
- Tests validate correctness of soft-posit operations on RISC-V architecture

### RISC-V Benchmarking
- Benchmarks can be run on RISC-V through QEMU emulation
- **Note**: Benchmark results under emulation don't reflect real hardware performance
- Useful for correctness validation and relative performance comparisons

### Running RISC-V Workflows
You can manually trigger RISC-V builds and tests:

1. **Via GitHub UI**: Go to Actions → "Run Tests" → Select "riscv" platform
2. **Via API**: Trigger workflow with platform input set to "riscv"

## Using the Local Development Workflow

The `local-dev.yml` workflow allows you to easily test changes locally. You can run it using the GitHub UI or with the `act` tool:

```bash
# Install act
brew install act

# Run the local-dev workflow with the 'act' tool
act -W .github/workflows/local-dev.yml -P ubuntu-latest=ghcr.io/catthehacker/ubuntu:act-latest --container-architecture linux/amd64 --input action=all
```

### Available Actions

The local-dev workflow supports the following actions:

- `build`: Just build without running tests
- `test`: Run all tests
- `benchmark`: Run benchmarks
- `all`: Run all of the above

## Build and Test Flow

Our workflows are designed with an efficient flow:

1. **Build jobs** create artifacts and cache the build
2. **Test jobs** use the cached builds to run tests
3. **Benchmark jobs** use the cached builds to run benchmarks

This separation allows for:
- Better workflow organization and maintenance
- Independent test and benchmark runs
- More efficient resource usage
- Cross-platform support (Ubuntu, macOS, RISC-V)

## Caching Strategy

- Incremental builds are efficiently cached based on source file hashes
- Different workflows reuse the same cache keys to avoid redundant builds
- Build artifacts are shared between jobs when needed
- Separate cache keys for different architectures (ubuntu, macos, riscv)

## Manual Build and Test

### Native Build (Ubuntu/macOS)
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j$(nproc)

# Run specific tests
./p8_tests
./p16_tests
./p32_tests
./q8_tests
./q16_tests
./q32_tests
```

### RISC-V Cross-Compilation
```bash
# Install RISC-V toolchain (Ubuntu)
sudo apt-get install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu qemu-user-static

# Configure for RISC-V
mkdir -p build-riscv
cd build-riscv
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
  -DCMAKE_C_COMPILER=riscv64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=riscv64-linux-gnu-g++ \
  -DCMAKE_FIND_ROOT_PATH=/usr/riscv64-linux-gnu \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY

make -j$(nproc)

# Run tests with QEMU
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p8_tests
qemu-riscv64-static -L /usr/riscv64-linux-gnu ./p16_tests
# ... etc for all test suites
```
