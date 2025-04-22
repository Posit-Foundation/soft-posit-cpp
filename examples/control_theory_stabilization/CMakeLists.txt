cmake_minimum_required(VERSION 3.10)
project(control_theory_stabilization CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SoftPosit headers
include_directories(${SOFTPOSIT_INCLUDE_DIRS})

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Define source files
set(CONTROL_SOURCES control_stabilization.cpp)

# Static version
add_executable(control_stabilization ${CONTROL_SOURCES})
target_link_libraries(control_stabilization ${SOFTPOSIT_LIBRARIES} m)

# Dynamic version (if shared libraries are available)
if(EXISTS "${SOFTPOSIT_SHARED_LIBRARIES}")
    add_executable(control_stabilization_dynamic ${CONTROL_SOURCES})
    target_link_libraries(control_stabilization_dynamic ${SOFTPOSIT_SHARED_LIBRARIES} m)
endif()

# Install
install(TARGETS control_stabilization
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT examples)

if(TARGET control_stabilization_dynamic)
    install(TARGETS control_stabilization_dynamic
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT examples)
endif()

# Create a custom target to build all control_theory_stabilization examples
add_custom_target(control_theory_stabilization_examples ALL 
                  DEPENDS control_stabilization)

if(TARGET control_stabilization_dynamic)
    add_dependencies(control_theory_stabilization_examples control_stabilization_dynamic)
endif() 