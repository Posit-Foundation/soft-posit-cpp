cmake_minimum_required(VERSION 3.10)
project(mandelbrot_example)

# Add executable
add_executable(mandelbrot mandelbrot.cpp)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 