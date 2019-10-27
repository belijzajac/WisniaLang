add_subdirectory(token)

set(MEGUMIN_SOURCES
  ${MEGUMIN_SOURCES}
  frontend/lexer/Lexer.h
  frontend/lexer/Lexer.cpp
  PARENT_SCOPE
)
