add_subdirectory(ast)

set(MEGUMIN_SOURCES
  ${MEGUMIN_SOURCES}
  frontend/parser/Parser.h
  frontend/parser/Parser.cpp
  PARENT_SCOPE
)
