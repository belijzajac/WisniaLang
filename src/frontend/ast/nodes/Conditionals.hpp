// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_CONDITIONALS_HPP
#define WISNIALANG_AST_CONDITIONALS_HPP

#include <utility>
// Wisnia
#include "Root.hpp"
#include "Statements.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class BaseIf : public BaseStmt {
  using StatementPtr = std::unique_ptr<BaseStmt>;

 public:
  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level);
  }

  void addBody(StatementPtr body) {
    m_body = std::move(body);
  }

  const StatementPtr &getBody() const {
    return m_body;
  }

 protected:
  explicit BaseIf(TokenPtr token)
      : BaseStmt(std::move(token)) {}

 protected:
  StatementPtr m_body;
};

class IfStmt final : public BaseIf {
  using ElseStatementPtr = std::unique_ptr<BaseIf>;
  using ConditionPtr     = std::unique_ptr<BaseExpr>;

 public:
  explicit IfStmt(TokenPtr token)
      : BaseIf(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "IfStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_condition->print(output, level);
    m_body->print(output, level);
    level--; // reset for else statements
    for (const auto &stmt : m_statements) {
      stmt->print(output, level);
    }
  }

  void addCondition(ConditionPtr condition) {
    m_condition = std::move(condition);
  }

  void addElseStatements(std::vector<ElseStatementPtr> statements) {
    m_statements = std::move(statements);
  }

  const ConditionPtr &getCondition() const {
    return m_condition;
  }

  const std::vector<ElseStatementPtr> &getElseStatements() const {
    return m_statements;
  }

 private:
  ConditionPtr m_condition;
  std::vector<ElseStatementPtr> m_statements;
};

class ElseStmt final : public BaseIf {
 public:
  explicit ElseStmt(TokenPtr token)
      : BaseIf(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ElseStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_body->print(output, level);
  }
};

class ElseIfStmt final : public BaseIf {
  using ConditionPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit ElseIfStmt(TokenPtr token)
      : BaseIf(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ElseIfStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_condition->print(output, level);
    m_body->print(output, level);
  }

  void addCondition(ConditionPtr expr) {
    m_condition = std::move(expr);
  }

  const ConditionPtr &getCondition() const {
    return m_condition;
  }

 private:
  ConditionPtr m_condition;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_CONDITIONALS_HPP
