cmake_minimum_required(VERSION 3.10)
project(multiplication_closure CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Print library paths for debugging
message(STATUS "Building multiplication_closure example")
message(STATUS "SOFTPOSIT_INCLUDE_DIRS: ${SOFTPOSIT_INCLUDE_DIRS}")

# Add library search path if needed
if(CMAKE_PREFIX_PATH)
  link_directories(${CMAKE_PREFIX_PATH}/lib)
endif()

# Dynamic linking version
add_executable(multiplication_closure_dynamic multiplication_closure.cpp)

# Link against shared library
target_link_libraries(multiplication_closure_dynamic ${SOFTPOSIT_SHARED_LIBRARIES})

# Include directories
target_include_directories(multiplication_closure_dynamic PRIVATE ${SOFTPOSIT_INCLUDE_DIRS})

# Static linking version
add_executable(multiplication_closure_static multiplication_closure.cpp)

# Link against static library
target_link_libraries(multiplication_closure_static ${SOFTPOSIT_LIBRARIES})

# Include directories
target_include_directories(multiplication_closure_static PRIVATE ${SOFTPOSIT_INCLUDE_DIRS}) 