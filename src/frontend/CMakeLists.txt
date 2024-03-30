# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(sema)
add_subdirectory(basic)

set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  PARENT_SCOPE
)
