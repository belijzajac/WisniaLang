# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

add_subdirectory(intermediate)
add_subdirectory(register)
add_subdirectory(optimize)
add_subdirectory(elf)
add_subdirectory(codegen)

set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  PARENT_SCOPE
)
