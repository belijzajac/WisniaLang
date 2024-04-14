// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_AST_STATEMENTS_HPP
#define WISNIALANG_AST_STATEMENTS_HPP

#include <string>
#include <utility>
// Wisnia
#include "Root.hpp"
#include "Types.hpp"
#include "Variable.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class BaseStmt : public Root {
 public:
  BaseStmt() = default;

  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level);
  }

 protected:
  explicit BaseStmt(TokenPtr token)
      : Root(std::move(token)) {}
};

class StmtBlock : public BaseStmt {
  using StatementPtr = std::unique_ptr<BaseStmt>;

 public:
  explicit StmtBlock(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  StmtBlock() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "StmtBlock";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    for (const auto &stmt : m_statements) {
      stmt->print(output, level);
    }
  }

  void addStatement(StatementPtr statement) {
    m_statements.push_back(std::move(statement));
  }

  const std::vector<StatementPtr> &getStatements() const {
    return m_statements;
  }

 private:
  std::vector<StatementPtr> m_statements;
};

class ReturnStmt : public BaseStmt {
  using ReturnValuePtr = std::unique_ptr<BaseExpr>;

 public:
  explicit ReturnStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  ReturnStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ReturnStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    m_returnValue->print(output, level);
  }

  void addReturnValue(ReturnValuePtr returnValue) {
    m_returnValue = std::move(returnValue);
  }

  const ReturnValuePtr &getReturnValue() const {
    return m_returnValue;
  }

 private:
  ReturnValuePtr m_returnValue;
};

class BreakStmt : public BaseStmt {
 public:
  explicit BreakStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "BreakStmt";
  }
};

class ContinueStmt : public BaseStmt {
 public:
  explicit ContinueStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ContinueStmt";
  }
};

class VarDeclStmt : public BaseStmt, public VariableMixin {
  using ValuePtr = std::unique_ptr<BaseExpr>;

 public:
  explicit VarDeclStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  VarDeclStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "VarDeclStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    getVariable()->print(output, level);
    m_value->print(output, level);
  }

  void addValue(ValuePtr varValue) {
    m_value = std::move(varValue);
  }

  const ValuePtr &getValue() const {
    return m_value;
  }

 private:
  ValuePtr m_value;
};

class VarAssignStmt : public BaseStmt, public VariableMixin {
  using ValuePtr = std::unique_ptr<BaseExpr>;

 public:
  explicit VarAssignStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  VarAssignStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "VarAssignStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    getVariable()->print(output, level);
    m_value->print(output, level);
  }

  void addValue(ValuePtr value) {
    m_value = std::move(value);
  }

  const ValuePtr &getValue() const {
    return m_value;
  }

 private:
  ValuePtr m_value;
};

class ExprStmt : public BaseStmt {
  using ExpressionPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit ExprStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  ExprStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ExprStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    m_expression->print(output, level);
  }

  void addExpression(ExpressionPtr expression) {
    m_expression = std::move(expression);
  }

  const ExpressionPtr &getExpression() const {
    return m_expression;
  }

 private:
  ExpressionPtr m_expression;
};

class ReadStmt : public BaseStmt, public VariableMixin {
 public:
  explicit ReadStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  ReadStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ReadStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    for (const auto &var : m_variables) {
      var->print(output, level);
    }
  }
};

class WriteStmt : public BaseStmt {
  using ExpressionPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit WriteStmt(TokenPtr token)
      : BaseStmt(std::move(token)) {}

  WriteStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "WriteStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    for (const auto &expr : m_expressions) {
      expr->print(output, level);
    }
  }

  void addExpression(ExpressionPtr expression) {
    m_expressions.push_back(std::move(expression));
  }

  const std::vector<ExpressionPtr> &getExpressions() const {
    return m_expressions;
  }

 private:
  std::vector<ExpressionPtr> m_expressions;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_STATEMENTS_HPP
