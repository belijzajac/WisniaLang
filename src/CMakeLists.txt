set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  main.cpp
)

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

set(WISNIA_TARGET "WisniaLang")

add_executable(${WISNIA_TARGET}
  ${WISNIA_SOURCES}
)
