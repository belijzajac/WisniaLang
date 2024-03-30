// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_SYMBOL_TABLE_HPP
#define WISNIALANG_SYMBOL_TABLE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <fmt/format.h>
// Wisnia
#include "Exceptions.hpp"

namespace Wisnia {
namespace AST {
class VarExpr;
}  // namespace AST

class SymbolTable {
 public:
  SymbolTable() {
    m_table = std::make_unique<ScopedSymbolTable>();
  };

  void addSymbol(AST::VarExpr *var);

  const AST::VarExpr *findSymbol(const std::string &name) const {
    return m_table->findSymbol(name);
  }

  void pushScope() {
    auto tmp = std::make_unique<ScopedSymbolTable>();
    tmp->m_parentScope = std::move(m_table);
    m_table = std::move(tmp);
  }

  void popScope() {
    m_table = std::move(m_table->m_parentScope);
  }

 private:
  struct ScopedSymbolTable {
    std::unordered_map<std::string, const AST::VarExpr *> m_info;
    std::unique_ptr<ScopedSymbolTable> m_parentScope;

    void addSymbol(const std::string &name, const AST::VarExpr *var) {
      m_info[name] = var;
    }

    const AST::VarExpr *findSymbol(const std::string &name) const {
      if (auto search = m_info.find(name); search != m_info.end())
        return search->second;
      if (m_parentScope) {
        return m_parentScope->findSymbol(name);
      }
      throw SemanticError{fmt::format("Name `{}` is not known in the current scope", name)};
    }
  };

  std::unique_ptr<ScopedSymbolTable> m_table;
};

}  // namespace Wisnia

#endif  // WISNIALANG_SYMBOL_TABLE_HPP
