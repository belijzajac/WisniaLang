add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(sema)
add_subdirectory(basic)

set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  PARENT_SCOPE
)
