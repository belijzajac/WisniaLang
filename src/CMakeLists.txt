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

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(utilities)

# Make WISNIA_SOURCES visible to the root CMakeLists file
set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  PARENT_SCOPE
)

set(WISNIA_TARGET "wisnia")
add_executable(${WISNIA_TARGET}
  main.cpp
  ${WISNIA_SOURCES}
)
target_link_libraries(${WISNIA_TARGET}
  PRIVATE
  fmt::fmt
)
