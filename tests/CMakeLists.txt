if (unittests)
  include(GoogleTest)
  enable_testing()
  
  add_subdirectory(lexical-analysis)
  
  add_executable(wisnia-tests ${TEST_FILES})
  target_link_libraries(wisnia-tests gtest_main)
  gtest_discover_tests(wisnia-tests)
endif()
