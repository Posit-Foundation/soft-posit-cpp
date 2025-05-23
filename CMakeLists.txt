cmake_minimum_required(VERSION 3.10)
project(SoftPosit VERSION 1.0.0 LANGUAGES C CXX)

# Options
option(SOFTPOSIT_QUAD "Enable quad-precision support" OFF)
option(SOFTPOSIT_FAST_INT64 "Enable fast int64 operations" ON)
option(BUILD_PYTHON_WRAPPER "Build Python wrapper" ON)
option(BUILD_PYTHON3_WRAPPER "Build Python3 wrapper" OFF)
option(BUILD_JULIA_WRAPPER "Build Julia wrapper" OFF)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_EXPERIMENTAL_TESTS "Enable experimental tests that may fail" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(INLINE_LEVEL "Inline level for optimization" 5)

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()


# Architecture specialization
set(SPECIALIZE_TYPE "8086-SSE" CACHE STRING "Specialization type for SoftPosit")

# Compiler options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

# Define include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/${SPECIALIZE_TYPE}
)

# Add compile definitions
add_compile_definitions(
  INLINE_LEVEL=${INLINE_LEVEL}
  SOFTPOSIT_FAST_INT64
)

# Add quad precision if enabled
if(SOFTPOSIT_QUAD)
  add_compile_definitions(SOFTPOSIT_QUAD)
  find_library(QUADMATH_LIBRARY quadmath)
  if(QUADMATH_LIBRARY)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${QUADMATH_LIBRARY})
  endif()
endif()

# Collect all source files
file(GLOB SOFTPOSIT_SOURCES
  "src/*.c"
  "src/*.cpp"
)

# Create libraries based on BUILD_SHARED_LIBS option
if(BUILD_SHARED_LIBS)
  add_library(softposit_shared SHARED ${SOFTPOSIT_SOURCES})
  set_target_properties(softposit_shared PROPERTIES OUTPUT_NAME softposit)
  target_link_libraries(softposit_shared ${EXTRA_LIBS})
  install(TARGETS softposit_shared
    EXPORT SoftPositTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
  )
endif()

# Always build static library
add_library(softposit STATIC ${SOFTPOSIT_SOURCES})
set_target_properties(softposit PROPERTIES OUTPUT_NAME softposit)
target_link_libraries(softposit ${EXTRA_LIBS})
install(TARGETS softposit
  EXPORT SoftPositTargets
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY include/ DESTINATION include)

# Create and install package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/SoftPositConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SoftPositConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SoftPositConfig.cmake"
  INSTALL_DESTINATION lib/cmake/SoftPosit
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SoftPositConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/SoftPositConfigVersion.cmake"
  DESTINATION lib/cmake/SoftPosit
)

# Export targets
install(EXPORT SoftPositTargets
  FILE SoftPositTargets.cmake
  NAMESPACE SoftPosit::
  DESTINATION lib/cmake/SoftPosit
)

# Python wrapper
if(BUILD_PYTHON_WRAPPER OR BUILD_PYTHON3_WRAPPER)
  # Fix CMP0078 and CMP0086 policy warnings
  if(POLICY CMP0078)
    cmake_policy(SET CMP0078 NEW)
  endif()
  
  if(POLICY CMP0086)
    cmake_policy(SET CMP0086 NEW)
  endif()

  # Find SWIG
  find_package(SWIG 3.0 REQUIRED)
  include(${SWIG_USE_FILE})
  
  # Find Python
  if(BUILD_PYTHON_WRAPPER)
    find_package(Python2 COMPONENTS Interpreter Development)
    if(Python2_FOUND)
      set(PYTHON_INCLUDE_DIRS ${Python2_INCLUDE_DIRS})
      set(PYTHON_LIBRARIES ${Python2_LIBRARIES})
      set(PYTHON_EXECUTABLE ${Python2_EXECUTABLE})
    else()
      message(FATAL_ERROR "Python2 not found. Please install Python2 or set BUILD_PYTHON_WRAPPER=OFF.")
    endif()
  else()
    find_package(Python3 COMPONENTS Interpreter Development)
    if(Python3_FOUND)
      set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
      set(PYTHON_LIBRARIES ${Python3_LIBRARIES})
      set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    else()
      find_package(PythonInterp 3)
      find_package(PythonLibs 3)
      if(PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND)
        set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
        set(PYTHON_LIBRARIES ${PYTHON_LIBRARIES})
        set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})
      else()
        message(FATAL_ERROR "Python3 not found. Please install Python3 or set BUILD_PYTHON3_WRAPPER=OFF.")
      endif()
    endif()
  endif()

  # Find numpy (optional)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
    RESULT_VARIABLE NUMPY_RESULT
    OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NUMPY_RESULT EQUAL 0)
    set(NUMPY_FOUND TRUE)
    include_directories(${NUMPY_INCLUDE_DIR})
  endif()

  # Set SWIG options
  set(CMAKE_SWIG_FLAGS "")
  include_directories(${PYTHON_INCLUDE_DIRS})

  # Set the source file for SWIG
  set_source_files_properties(python/softposit_python.i PROPERTIES CPLUSPLUS OFF)
  set_source_files_properties(python/softposit_python.i PROPERTIES SWIG_FLAGS "-includeall")

  # Create the SWIG Python module
  # Use a direct method to compile the wrapper file
  execute_process(
    COMMAND ${SWIG_EXECUTABLE} -python -o ${CMAKE_CURRENT_SOURCE_DIR}/python/softposit_python_wrap.c ${CMAKE_CURRENT_SOURCE_DIR}/python/softposit_python.i
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  # Now build the Python extension module
  add_library(softposit_python MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/python/softposit_python_wrap.c
  )
  target_include_directories(softposit_python PRIVATE ${PYTHON_INCLUDE_DIRS})
  target_link_libraries(softposit_python softposit ${PYTHON_LIBRARIES})

  # Rename the output file and set the output directory
  set_target_properties(softposit_python PROPERTIES 
    PREFIX ""
    OUTPUT_NAME "_softposit"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/python"
  )

  # Install Python module
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(plat_specific=True))"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  if(PYTHON_SITE_PACKAGES)
    install(FILES 
      "${CMAKE_CURRENT_SOURCE_DIR}/python/softposit.py" 
      DESTINATION ${PYTHON_SITE_PACKAGES}
    )
    
    install(TARGETS softposit_python
      LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
    )
  endif()
endif()

# Tests setup
if(BUILD_TESTS)
  # Download and configure Google Test
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  # Enable testing
  enable_testing()

  # Ensure tests directory exists
  set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  if(NOT EXISTS "${TESTS_DIR}")
    file(MAKE_DIRECTORY "${TESTS_DIR}")
    message(STATUS "Created tests directory: ${TESTS_DIR}")
  endif()

  # Common test utilities
  set(COMMON_TEST_SOURCES
    "${TESTS_DIR}/common_tests/test_utils.cpp"
  )

  # Add header files
  set(COMMON_TEST_HEADERS
    "${TESTS_DIR}/common_tests/test_utils.h"
  )

  # Include the tests directory in the include path
  include_directories(${TESTS_DIR})

  # Function to add test executable with auto-discovered test files
  function(add_posit_test_executable NAME SIZE)
    # Auto-discover test files in the directory
    file(GLOB TEST_SOURCES "${TESTS_DIR}/${NAME}_tests/*.cpp")
    
    # Add the test executable with all discovered files
    add_executable(${NAME}_tests 
      ${TEST_SOURCES}
      "${TESTS_DIR}/main.cpp"
      ${COMMON_TEST_SOURCES}
    )
    target_link_libraries(${NAME}_tests gtest gtest_main pthread softposit)
    
    # Add the experimental tests flag if enabled
    if(ENABLE_EXPERIMENTAL_TESTS)
      target_compile_definitions(${NAME}_tests PRIVATE ENABLE_EXPERIMENTAL_TESTS)
    endif()
    
    add_test(NAME ${NAME}_tests COMMAND ${NAME}_tests)
  endfunction()

  # Add all posit and quire tests
  add_posit_test_executable(p8 8)
  add_posit_test_executable(p16 16)
  add_posit_test_executable(p32 32)
  add_posit_test_executable(q8 8)
  add_posit_test_executable(q16 16)
  add_posit_test_executable(q32 32)

  # Add a custom target to run all tests
  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS p8_tests p16_tests p32_tests q8_tests q16_tests q32_tests
  )
endif()

# Benchmarks setup
if(BUILD_BENCHMARKS)
  # Download and configure Google Benchmark
  include(FetchContent)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
  )
  # Disable benchmark tests
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
  FetchContent_MakeAvailable(benchmark)

  # Ensure benchmark directory exists
  set(BENCHMARKS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")
  if(NOT EXISTS "${BENCHMARKS_DIR}")
    file(MAKE_DIRECTORY "${BENCHMARKS_DIR}")
    file(MAKE_DIRECTORY "${BENCHMARKS_DIR}/performance")
    message(STATUS "Created benchmarks directory: ${BENCHMARKS_DIR}")
  endif()

  # Common benchmark utilities
  set(COMMON_BENCH_SOURCES
    "${TESTS_DIR}/common_tests/test_utils.cpp"  # Reuse test utilities for benchmarks
  )

  # Ensure benchmarks can find Google Test headers
  include_directories(${gtest_SOURCE_DIR}/include)

  # Function to add benchmark executable if the file exists
  function(add_benchmark_if_exists NAME)
    set(BENCH_FILE "${BENCHMARKS_DIR}/performance/${NAME}_bench.cpp")
    if(EXISTS "${BENCH_FILE}")
      add_executable(${NAME}_bench 
        "${BENCH_FILE}"
        ${COMMON_BENCH_SOURCES}
      )
      target_link_libraries(${NAME}_bench benchmark::benchmark softposit pthread)
      
      # Add individual benchmark run target
      add_custom_target(run-${NAME}-bench
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_bench
        DEPENDS ${NAME}_bench
      )
      set_property(GLOBAL APPEND PROPERTY BENCHMARK_TARGETS run-${NAME}-bench)
    endif()
  endfunction()

  # Add benchmarks for all posit sizes
  add_benchmark_if_exists(p8)
  add_benchmark_if_exists(p16)
  add_benchmark_if_exists(p32)
  add_benchmark_if_exists(q8)
  add_benchmark_if_exists(q16)
  add_benchmark_if_exists(q32)

  # Get all benchmark targets
  get_property(BENCHMARK_TARGETS GLOBAL PROPERTY BENCHMARK_TARGETS)

  # Add a custom target to run all benchmarks
  add_custom_target(bench
    COMMAND ${CMAKE_COMMAND} -E echo "Running all benchmarks..."
  )
  
  # Add dependencies to the bench target
  if(BENCHMARK_TARGETS)
    add_dependencies(bench ${BENCHMARK_TARGETS})
  endif()
endif()

# Add clean target
add_custom_target(clean-all
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_clean.cmake
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmake_clean.cmake
"
file(REMOVE_RECURSE
  \${CMAKE_BINARY_DIR}/CMakeFiles
  \${CMAKE_BINARY_DIR}/CMakeCache.txt
  \${CMAKE_BINARY_DIR}/cmake_install.cmake
  \${CMAKE_BINARY_DIR}/Makefile
  \${CMAKE_BINARY_DIR}/libsoftposit.a
  \${CMAKE_BINARY_DIR}/libsoftposit.so
  \${CMAKE_CURRENT_SOURCE_DIR}/python/_softposit.so
  \${CMAKE_BINARY_DIR}/p8_tests
  \${CMAKE_BINARY_DIR}/p16_tests
  \${CMAKE_BINARY_DIR}/p32_tests
  \${CMAKE_BINARY_DIR}/q8_tests
  \${CMAKE_BINARY_DIR}/q16_tests
  \${CMAKE_BINARY_DIR}/q32_tests
  \${CMAKE_BINARY_DIR}/p8_bench
  \${CMAKE_BINARY_DIR}/p16_bench
  \${CMAKE_BINARY_DIR}/p32_bench
  \${CMAKE_BINARY_DIR}/q8_bench
  \${CMAKE_BINARY_DIR}/q16_bench
  \${CMAKE_BINARY_DIR}/q32_bench
)
"
)

# Package configuration
set(CPACK_PACKAGE_NAME "SoftPosit")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "SoftPosit - A C++ implementation of the Posit number system")
set(CPACK_PACKAGE_VENDOR "SoftPosit Team")
set(CPACK_PACKAGE_CONTACT "softposit@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Platform-specific settings
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SoftPosit")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SoftPosit Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.14)")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.14")
endif()

# Include CPack
include(CPack)