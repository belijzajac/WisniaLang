set(MEGUMIN_SOURCES
  ${MEGUMIN_SOURCES}
  main.cpp
)

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

set(MEGUMIN_TARGET "Megumin")

add_executable(${MEGUMIN_TARGET}
  ${MEGUMIN_SOURCES}
)
