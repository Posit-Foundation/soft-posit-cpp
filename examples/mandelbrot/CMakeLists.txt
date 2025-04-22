cmake_minimum_required(VERSION 3.10)
project(mandelbrot CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the SoftPosit package
find_package(SoftPosit REQUIRED)

# Print library paths for debugging
message(STATUS "SOFTPOSIT_INCLUDE_DIRS: ${SOFTPOSIT_INCLUDE_DIRS}")
message(STATUS "SOFTPOSIT_LIBRARIES: ${SOFTPOSIT_LIBRARIES}")
message(STATUS "SOFTPOSIT_SHARED_LIBRARIES: ${SOFTPOSIT_SHARED_LIBRARIES}")

# Add library search path
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

# Dynamic linking version
add_executable(mandelbrot_dynamic mandelbrot.cpp)
add_executable(posit_mandelbrot_dynamic posit_mandelbrot.cpp)

# Link against shared library
target_link_libraries(mandelbrot_dynamic ${SOFTPOSIT_SHARED_LIBRARIES})
target_link_libraries(posit_mandelbrot_dynamic ${SOFTPOSIT_SHARED_LIBRARIES})

# Include directories
target_include_directories(mandelbrot_dynamic PRIVATE ${SOFTPOSIT_INCLUDE_DIRS})
target_include_directories(posit_mandelbrot_dynamic PRIVATE ${SOFTPOSIT_INCLUDE_DIRS})

# Static linking version
add_executable(mandelbrot_static mandelbrot.cpp)
add_executable(posit_mandelbrot_static posit_mandelbrot.cpp)

# Link against static library
target_link_libraries(mandelbrot_static ${SOFTPOSIT_LIBRARIES})
target_link_libraries(posit_mandelbrot_static ${SOFTPOSIT_LIBRARIES})

# Include directories
target_include_directories(mandelbrot_static PRIVATE ${SOFTPOSIT_INCLUDE_DIRS})
target_include_directories(posit_mandelbrot_static PRIVATE ${SOFTPOSIT_INCLUDE_DIRS}) 