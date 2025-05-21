# GitHub Actions Workflows

This guide explains the GitHub Actions workflows for the soft-posit-cpp project.

## Workflow Structure

We've organized our workflows for better clarity and efficiency:

1. **Build Workflows**:
   - `build-ubuntu.yml`: Builds the project on Ubuntu
   - `build-macos.yml`: Builds the project on macOS

2. **Testing & Benchmarking**:
   - `tests.yml`: Runs tests on cached builds (supports both platforms)
   - `benchmarks.yml`: Runs benchmarks on Ubuntu with cached builds

3. **Code Quality**:
   - `code-quality.yml`: Runs clang-format and clang-tidy checks

4. **Release**:
   - `release.yml`: Creates packages and releases for tagged versions

5. **Development**:
   - `local-dev.yml`: Simplified workflow for local development and testing

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

## Caching Strategy

- Incremental builds are efficiently cached based on source file hashes
- Different workflows reuse the same cache keys to avoid redundant builds
- Build artifacts are shared between jobs when needed

## Manual Build and Test

Alternatively, you can build and test the project directly:

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
