include(FetchContent)

if(UNIT_TESTS)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        9741c42200b66abc708ee6da269a29c8bd912cee
  )
  FetchContent_MakeAvailable(googletest)
endif()

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        8.0.1
)
FetchContent_MakeAvailable(fmt)
