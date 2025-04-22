# FindSoftPosit.cmake
#
# Finds the SoftPosit library
#
# This will define the following variables:
#   SOFTPOSIT_FOUND        - True if SoftPosit is found
#   SOFTPOSIT_INCLUDE_DIRS - Location of SoftPosit headers
#   SOFTPOSIT_LIBRARIES    - SoftPosit static libraries
#   SOFTPOSIT_SHARED_LIBRARIES - SoftPosit shared libraries

# Try common include paths
find_path(SOFTPOSIT_INCLUDE_DIR
  NAMES softposit.h
  PATHS
    ${CMAKE_PREFIX_PATH}/include
    /usr/local/include
    /usr/include
    ../../include
    ../include
    ../../install/include
    ../install/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
  DOC "SoftPosit include directory"
)

# Try to find the static library
find_library(SOFTPOSIT_LIBRARY
  NAMES softposit libsoftposit
  PATHS
    ${CMAKE_PREFIX_PATH}/lib
    /usr/local/lib
    /usr/lib
    ../../lib
    ../lib
    ../../install/lib
    ../install/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
  DOC "SoftPosit static library"
)

# Try to find the shared library
find_library(SOFTPOSIT_SHARED_LIBRARY
  NAMES softposit libsoftposit
  PATHS
    ${CMAKE_PREFIX_PATH}/lib
    /usr/local/lib
    /usr/lib
    ../../lib
    ../lib
    ../../install/lib
    ../install/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
  DOC "SoftPosit shared library"
)

# Set variables for found components
if(SOFTPOSIT_INCLUDE_DIR)
  set(SOFTPOSIT_INCLUDE_DIRS ${SOFTPOSIT_INCLUDE_DIR})
endif()

if(SOFTPOSIT_LIBRARY)
  set(SOFTPOSIT_LIBRARIES ${SOFTPOSIT_LIBRARY})
endif()

if(SOFTPOSIT_SHARED_LIBRARY)
  set(SOFTPOSIT_SHARED_LIBRARIES ${SOFTPOSIT_SHARED_LIBRARY})
else()
  # If shared library not found, use static library
  set(SOFTPOSIT_SHARED_LIBRARIES ${SOFTPOSIT_LIBRARIES})
endif()

# Print status messages for debugging
message(STATUS "SOFTPOSIT_INCLUDE_DIR: ${SOFTPOSIT_INCLUDE_DIR}")
message(STATUS "SOFTPOSIT_LIBRARY: ${SOFTPOSIT_LIBRARY}")
message(STATUS "SOFTPOSIT_SHARED_LIBRARY: ${SOFTPOSIT_SHARED_LIBRARY}")

# Handle REQUIRED and QUIET arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SoftPosit
  REQUIRED_VARS SOFTPOSIT_INCLUDE_DIR
  FAIL_MESSAGE "SoftPosit not found, please set CMAKE_PREFIX_PATH to the installation directory"
)

# Mark variables as advanced
mark_as_advanced(SOFTPOSIT_INCLUDE_DIR SOFTPOSIT_LIBRARY SOFTPOSIT_SHARED_LIBRARY) 