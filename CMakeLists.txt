cmake_minimum_required(VERSION 3.13)
project(WisniaLang)

# We're using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate cmake helper modules
list(APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
)

# Include the said modules
include(opts)
include(deps)

# In order to have shorter paths to header files.
# Defined here because unit tests need to access them too.
include_directories(
  src/utilities/
  src/frontend/basic/
  src/frontend/ast/
  src/frontend/lexer/
  src/frontend/parser/
)

add_subdirectory(src)
add_subdirectory(tests)
