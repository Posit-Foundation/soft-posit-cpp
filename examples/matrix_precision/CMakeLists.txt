cmake_minimum_required(VERSION 3.10)
project(matrix_precision CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SoftPosit headers
include_directories(${SOFTPOSIT_INCLUDE_DIRS})

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Define source files
set(MATRIX_PRECISION_SOURCES matrix_multiplication.cpp)

# Static version
add_executable(matrix_precision ${MATRIX_PRECISION_SOURCES})
target_link_libraries(matrix_precision ${SOFTPOSIT_LIBRARIES} m)

# Dynamic version (if shared libraries are available)
if(EXISTS "${SOFTPOSIT_SHARED_LIBRARIES}")
    add_executable(matrix_precision_dynamic ${MATRIX_PRECISION_SOURCES})
    target_link_libraries(matrix_precision_dynamic ${SOFTPOSIT_SHARED_LIBRARIES} m)
endif()

# Install
install(TARGETS matrix_precision
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT examples)

if(TARGET matrix_precision_dynamic)
    install(TARGETS matrix_precision_dynamic
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT examples)
endif()

# Create a custom target to build all matrix_precision examples
add_custom_target(matrix_precision_examples ALL 
                  DEPENDS matrix_precision)

if(TARGET matrix_precision_dynamic)
    add_dependencies(matrix_precision_examples matrix_precision_dynamic)
endif() 