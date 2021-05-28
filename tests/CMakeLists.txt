if (unittests)
  include(GoogleTest)
  enable_testing()
  
  add_subdirectory(lexical-analysis)
  
  add_executable(unittests ${TEST_FILES})
  target_link_libraries(unittests gtest_main)  
  gtest_discover_tests(unittests)
endif()
