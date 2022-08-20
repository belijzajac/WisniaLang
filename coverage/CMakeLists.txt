# WisniaLang - A Compiler for an Experimental Programming Language
# Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

if(CMAKE_BUILD_TYPE STREQUAL "Coverage" AND CMAKE_COMPILER_IS_GNUCC)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  setup_target_for_coverage_lcov(
    NAME wisnia-coverage
    EXECUTABLE wisnia-tests
    EXCLUDE
      "/usr/*"
      "${PROJECT_SOURCE_DIR}/tests/*"
      "${PROJECT_SOURCE_DIR}/build/_deps/*"
  )
endif()
