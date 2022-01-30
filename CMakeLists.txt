cmake_minimum_required(VERSION 3.13)
project(WisniaLang)

option(UNIT_TESTS "Build all of wisnia's unit tests" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  set(UNIT_TESTS ON)
endif()

# We're using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate CMake helper modules
list(APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
)

# Include the said modules
include(Dependencies)

# In order to have shorter paths to header files
# Defined here because unit tests need to access them too
include_directories(
# utilities
  src/utilities/
# frontend
  src/frontend/basic/
  src/frontend/ast/
  src/frontend/lexer/
  src/frontend/parser/
  src/frontend/sema/
# backend
  src/backend/ir/
)

add_subdirectory(src)
add_subdirectory(tests)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage" AND CMAKE_COMPILER_IS_GNUCC)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  setup_target_for_coverage_lcov(
    NAME wisnia-coverage
    EXECUTABLE wisnia-tests
    EXCLUDE
      "/usr/*"
      "${PROJECT_SOURCE_DIR}/tests/*"
      "${PROJECT_SOURCE_DIR}/build/_deps/*"
  )
endif()
