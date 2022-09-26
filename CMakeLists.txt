# WisniaLang - A Compiler for an Experimental Programming Language
# Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.13)
project(WisniaLang)

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
# utilities
  src/utilities/
# frontend
  src/frontend/basic/
  src/frontend/ast/
  src/frontend/lexer/
  src/frontend/parser/
  src/frontend/sema/
# backend
  src/backend/intermediate/
  src/backend/register/
  src/backend/elf/
  src/backend/codegen/
)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(coverage)
