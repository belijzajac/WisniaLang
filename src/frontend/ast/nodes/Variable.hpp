// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_AST_VARIABLE_HPP
#define WISNIALANG_AST_VARIABLE_HPP

#include <utility>

namespace Wisnia {
namespace AST {

class BaseExpr;
using VariablePtr = std::unique_ptr<BaseExpr>;

class VariableMixin {
 public:
  void addVariable(VariablePtr variable) {
    m_variables.push_back(std::move(variable));
  }

  const VariablePtr &getVariable() const {
    return m_variables.back();
  }

  const std::vector<VariablePtr> &getVariableList() const {
    return m_variables;
  }

 protected:
  std::vector<VariablePtr> m_variables;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_VARIABLE_HPP
