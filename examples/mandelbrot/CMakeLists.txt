cmake_minimum_required(VERSION 3.10)
project(mandelbrot_example)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the posit library
add_subdirectory(${CMAKE_SOURCE_DIR}/../.. ${CMAKE_BINARY_DIR}/posit)

# Add executables
add_executable(mandelbrot mandelbrot.cpp)
add_executable(posit_mandelbrot posit_mandelbrot.cpp)

# Include directories
target_include_directories(mandelbrot PRIVATE ${CMAKE_SOURCE_DIR}/../../include)
target_include_directories(posit_mandelbrot PRIVATE ${CMAKE_SOURCE_DIR}/../../include)

# Link posit library
target_link_libraries(posit_mandelbrot softposit) 