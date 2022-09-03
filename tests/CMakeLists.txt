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
  # programs
  add_subdirectory(programs)

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
