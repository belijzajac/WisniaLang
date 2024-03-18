# Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
# SPDX-License-Identifier: GPL-3.0

set(WISNIA_SOURCES
  ${WISNIA_SOURCES}
  backend/intermediate/IRGenerator.hpp
  backend/intermediate/IRGenerator.cpp
  backend/intermediate/Instruction.hpp
  backend/intermediate/Instruction.cpp
  backend/intermediate/Operation.hpp
  backend/intermediate/IRPrintHelper.hpp
  backend/intermediate/IRPrintHelper.cpp
  backend/intermediate/Modules.hpp
  backend/intermediate/Modules.cpp
  PARENT_SCOPE
)
