# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

# Appends "../src/" to the list of sources to form absolute paths
function(modify_sources SourceFiles)
  foreach(SRC IN LISTS WISNIA_SOURCES)
    set(SRC "../src/${SRC}")
    list(APPEND SRCS ${SRC})
  endforeach()
  set(${SourceFiles} ${SRCS} PARENT_SCOPE)
endfunction()

if(UNIT_TESTS)
  include(GoogleTest)
  enable_testing()

  # frontend
  add_subdirectory(lexical-analysis)
  add_subdirectory(syntax-analysis)
  add_subdirectory(semantic-analysis)
  # backend
  add_subdirectory(intermediate-representation)
  add_subdirectory(register-allocation)
  # programs
  add_subdirectory(programs)
  # utilities
  add_subdirectory(utilities)

  modify_sources(WISNIA_SOURCES)
  add_executable(wisnia-tests
    ${TEST_FILES}
    ${WISNIA_SOURCES}
  )
  target_link_libraries(wisnia-tests
    PRIVATE
    fmt::fmt
    gtest_main
  )
  gtest_discover_tests(wisnia-tests)
endif()
