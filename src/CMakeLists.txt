set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  main.cpp
  #PARENT_SCOPE
)

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

set(WISNIA_TARGET "wisnia")

add_executable(${WISNIA_TARGET}
  ${WISNIA_SOURCES}
)
