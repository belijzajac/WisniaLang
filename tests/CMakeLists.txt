if (unittests)
  include(GoogleTest)
  enable_testing()
  
  add_subdirectory(lexical-analysis)
  
  #add_executable(wisnia-tests ../src/${WISNIA_SOURCES} ${TEST_FILES})
  add_executable(wisnia-tests ${TEST_FILES} ../src/frontend/lexer/Lexer.h ../src/frontend/lexer/Lexer.cpp ../src/utilities/Exceptions.h ../src/frontend/basic/Token.h ../src/frontend/basic/TType.h)

  target_link_libraries(wisnia-tests gtest_main)
  gtest_discover_tests(wisnia-tests)
endif()
