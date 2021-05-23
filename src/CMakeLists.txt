set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  main.cpp
)

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

set(WISNIA_TARGET "wisnia")

add_executable(${WISNIA_TARGET}
  ${WISNIA_SOURCES}
)

# In order to have shorter paths to header files.
# Instead of having to write something like #include "../../lole.h",
# we would write #include "lole.h"
include_directories(
  ./utilities/
  ./frontend/basic/
  ./frontend/ast/
  ./frontend/lexer/
  ./frontend/parser/
)
