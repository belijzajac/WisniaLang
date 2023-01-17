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

FetchContent_Declare(lyra
  GIT_REPOSITORY https://github.com/belijzajac/Lyra
  GIT_TAG        74bc809d288d2222dc16d20f98c26711722400f7
)
FetchContent_MakeAvailable(lyra)
