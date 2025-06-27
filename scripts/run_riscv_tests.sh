#!/bin/bash
set -e

echo "======================================"
echo "Running RISC-V Tests with QEMU"
echo "======================================"

# Check if build directory exists
if [ ! -d "build-riscv" ]; then
    echo "Error: build-riscv directory not found."
    echo "Please run ./scripts/build_riscv.sh first to build the project."
    exit 1
fi

# Check if QEMU is available
if ! command -v qemu-riscv64-static &> /dev/null; then
    echo "Error: QEMU not found. Please install it with:"
    echo "  sudo apt-get install qemu-user-static"
    exit 1
fi

cd build-riscv

# Make sure executables are executable
chmod +x *_tests 2>/dev/null || true

# List of test executables
tests=("p8_tests" "p16_tests" "p32_tests" "q8_tests" "q16_tests" "q32_tests")

echo "Starting test execution..."
echo ""

# Run each test suite
for test in "${tests[@]}"; do
    if [ -f "./$test" ]; then
        echo "----------------------------------------"
        echo "Running $test..."
        echo "----------------------------------------"
        
        # Verify it's a RISC-V binary
        file_info=$(file "./$test")
        if [[ $file_info == *"RISC-V"* ]]; then
            echo "✓ Verified RISC-V binary: $test"
            start_time=$(date +%s)
            
            # Run the test with QEMU
            if qemu-riscv64-static -L /usr/riscv64-linux-gnu "./$test"; then
                end_time=$(date +%s)
                duration=$((end_time - start_time))
                echo "✓ $test passed (${duration}s)"
            else
                echo "✗ $test failed"
                exit 1
            fi
        else
            echo "✗ Warning: $test is not a RISC-V binary"
            echo "  File info: $file_info"
        fi
        echo ""
    else
        echo "✗ Test executable not found: $test"
        echo "  Make sure the project was built successfully."
    fi
done

echo "======================================"
echo "All tests completed successfully!"
echo "======================================" 