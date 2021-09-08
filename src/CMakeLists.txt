add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

# Make WISNIA_SOURCES visible to the root CMakeLists file
set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  PARENT_SCOPE
)

set(WISNIA_TARGET "wisnia")
add_executable(${WISNIA_TARGET} main.cpp ${WISNIA_SOURCES})

target_link_libraries(${WISNIA_TARGET}
  PRIVATE
  fmt::fmt
)
