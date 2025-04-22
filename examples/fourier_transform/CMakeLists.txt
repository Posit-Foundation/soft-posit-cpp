cmake_minimum_required(VERSION 3.10)
project(fourier_transform CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SoftPosit headers
include_directories(${SOFTPOSIT_INCLUDE_DIRS})

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Define source files
set(FOURIER_SOURCES fourier_transform.cpp)

# Static version
add_executable(fourier_transform ${FOURIER_SOURCES})
target_link_libraries(fourier_transform ${SOFTPOSIT_LIBRARIES} m)

# Dynamic version (if shared libraries are available)
if(EXISTS "${SOFTPOSIT_SHARED_LIBRARIES}")
    add_executable(fourier_transform_dynamic ${FOURIER_SOURCES})
    target_link_libraries(fourier_transform_dynamic ${SOFTPOSIT_SHARED_LIBRARIES} m)
endif()

# Install
install(TARGETS fourier_transform
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT examples)

if(TARGET fourier_transform_dynamic)
    install(TARGETS fourier_transform_dynamic
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT examples)
endif()

# Create a custom target to build all fourier_transform examples
add_custom_target(fourier_transform_examples ALL 
                  DEPENDS fourier_transform)

if(TARGET fourier_transform_dynamic)
    add_dependencies(fourier_transform_examples fourier_transform_dynamic)
endif() 