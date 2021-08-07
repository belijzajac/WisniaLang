# Appends "../src/" to the list of sources to form absolute paths.
function(modify_sources SourceFiles)
  foreach(SRC IN LISTS WISNIA_SOURCES)
    set(SRC "../src/${SRC}")
    list(APPEND SRCS ${SRC})
  endforeach()
  set(${SourceFiles} ${SRCS} PARENT_SCOPE)
endfunction()

if (unittests)
  include(GoogleTest)
  enable_testing()

  add_subdirectory(lexical-analysis)
  add_subdirectory(syntax-analysis)

  modify_sources(WISNIA_SOURCES)
  add_executable(wisnia-tests ${TEST_FILES} ${WISNIA_SOURCES})
  target_link_libraries(wisnia-tests gtest_main)
  gtest_discover_tests(wisnia-tests)
endif()
