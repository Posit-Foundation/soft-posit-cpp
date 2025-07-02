cmake_minimum_required(VERSION 3.10)
project(posit_examples CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add our custom cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Get SoftPosit location from CMAKE_PREFIX_PATH or use default path
if(NOT DEFINED CMAKE_PREFIX_PATH)
  message(STATUS "CMAKE_PREFIX_PATH not defined, using default ../../install")
  set(SOFTPOSIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install")
else()
  set(SOFTPOSIT_DIR "${CMAKE_PREFIX_PATH}")
endif()

# Directly set SoftPosit include and library paths
set(SOFTPOSIT_INCLUDE_DIRS "${SOFTPOSIT_DIR}/include")
set(SOFTPOSIT_LIBRARIES "${SOFTPOSIT_DIR}/lib/libsoftposit.a")
set(SOFTPOSIT_SHARED_LIBRARIES "${SOFTPOSIT_DIR}/lib/libsoftposit${CMAKE_SHARED_LIBRARY_SUFFIX}")

message(STATUS "Using SoftPosit from: ${SOFTPOSIT_DIR}")
message(STATUS "SOFTPOSIT_INCLUDE_DIRS: ${SOFTPOSIT_INCLUDE_DIRS}")
message(STATUS "SOFTPOSIT_LIBRARIES: ${SOFTPOSIT_LIBRARIES}")
message(STATUS "SOFTPOSIT_SHARED_LIBRARIES: ${SOFTPOSIT_SHARED_LIBRARIES}")

# Check if the include directory exists
# if(NOT EXISTS "${SOFTPOSIT_INCLUDE_DIRS}")
#   message(FATAL_ERROR "SoftPosit include directory not found: ${SOFTPOSIT_INCLUDE_DIRS}\n"
#                      "Please specify -DCMAKE_PREFIX_PATH=<path-to-softposit-install>")
# endif()

# Try to use find_package first, fall back to direct paths if it fails
find_package(SoftPosit QUIET)
if(NOT SOFTPOSIT_FOUND)
  message(STATUS "SoftPosit package not found via find_package, using direct paths")
endif()

# Add subdirectories
add_subdirectory(mandelbrot)
add_subdirectory(multiplication_closure)
add_subdirectory(julia_set_zoom)
add_subdirectory(matrix_precision)
add_subdirectory(control_theory_stabilization)
add_subdirectory(fourier_transform) 