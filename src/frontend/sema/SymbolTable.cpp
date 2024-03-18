// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include "SymbolTable.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace AST;

void SymbolTable::addSymbol(AST::VarExpr *var) {
  m_table->addSymbol(var->getToken()->getValue<std::string>(), var);
}
