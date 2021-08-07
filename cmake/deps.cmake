include(FetchContent)

if (unittests)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        9741c42200b66abc708ee6da269a29c8bd912cee
  )

  FetchContent_MakeAvailable(googletest)
endif()
