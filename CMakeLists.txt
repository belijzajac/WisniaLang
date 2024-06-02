# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

cmake_minimum_required(VERSION 3.25.1)
project(WisniaLang)

add_compile_definitions(WISNIA_VERSION="1.0.4-dev")

option(UNIT_TESTS "Build unit tests" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  set(UNIT_TESTS ON)
endif()

# We're using C++20
set(CMAKE_CXX_STANDARD 20)
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
  src/utilities/
  src/frontend/basic/
  src/frontend/ast/
  src/frontend/lexer/
  src/frontend/parser/
  src/frontend/sema/
  src/backend/intermediate/
  src/backend/register/
  src/backend/optimize/
  src/backend/elf/
  src/backend/codegen/
)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(coverage)
