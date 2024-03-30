# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

if(CMAKE_BUILD_TYPE STREQUAL "Coverage" AND CMAKE_COMPILER_IS_GNUCC)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  setup_target_for_coverage_lcov(
    NAME wisnia-coverage
    EXECUTABLE wisnia-tests
    EXCLUDE
      "/usr/*"
      "${PROJECT_SOURCE_DIR}/src/main.cpp"
      "${PROJECT_SOURCE_DIR}/tests/*"
      "${PROJECT_SOURCE_DIR}/build/_deps/*"
  )
endif()
