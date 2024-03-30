// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_AST_STATEMENTS_HPP
#define WISNIALANG_AST_STATEMENTS_HPP

#include <string>
// Wisnia
#include "Root.hpp"
#include "Types.hpp"

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
  explicit BaseStmt(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) {}
};

class StmtBlock : public BaseStmt {
 public:
  explicit StmtBlock(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

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

  void addStmt(std::unique_ptr<BaseStmt> stmt) {
    m_statements.push_back(std::move(stmt));
  }

  const std::vector<std::unique_ptr<BaseStmt>> &getStatements() const {
    return m_statements;
  }

 private:
  std::vector<std::unique_ptr<BaseStmt>> m_statements;
};

class ReturnStmt : public BaseStmt {
 public:
  explicit ReturnStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

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

  void addReturnValue(std::unique_ptr<BaseExpr> returnVal) {
    m_returnValue = std::move(returnVal);
  }

  const std::unique_ptr<BaseExpr> &getReturnValue() const {
    return m_returnValue;
  }

 private:
  std::unique_ptr<BaseExpr> m_returnValue;
};

class BreakStmt : public BaseStmt {
 public:
  explicit BreakStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "BreakStmt";
  }
};

class ContinueStmt : public BaseStmt {
 public:
  explicit ContinueStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ContinueStmt";
  }
};

class VarDeclStmt : public BaseStmt {
 public:
  explicit VarDeclStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  VarDeclStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "VarDeclStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    m_var->print(output, level);
    m_value->print(output, level);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  void addValue(std::unique_ptr<BaseExpr> varValue) {
    m_value = std::move(varValue);
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

  const std::unique_ptr<BaseExpr> &getValue() const {
    return m_value;
  }

 private:
  std::unique_ptr<BaseExpr> m_var;
  std::unique_ptr<BaseExpr> m_value;
};

class VarAssignStmt : public BaseStmt {
 public:
  explicit VarAssignStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  VarAssignStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "VarAssignStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    m_var->print(output, level);
    m_val->print(output, level);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  void addValue(std::unique_ptr<BaseExpr> varValue) {
    m_val = std::move(varValue);
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

  const std::unique_ptr<BaseExpr> &getValue() const {
    return m_val;
  }

 private:
  std::unique_ptr<BaseExpr> m_var;
  std::unique_ptr<BaseExpr> m_val;
};

class ExprStmt : public BaseStmt {
 public:
  explicit ExprStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  ExprStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ExprStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    m_expr->print(output, level);
  }

  void addExpr(std::unique_ptr<BaseExpr> expr) {
    m_expr = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getExpr() const {
    return m_expr;
  }

 private:
  std::unique_ptr<BaseExpr> m_expr;
};

class ReadStmt : public BaseStmt {
 public:
  explicit ReadStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  ReadStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ReadStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    for (const auto &var : m_vars) {
      var->print(output, level);
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_vars.push_back(std::move(var));
  }

  const std::vector<std::unique_ptr<BaseExpr>> &getVars() const {
    return m_vars;
  }

 private:
  std::vector<std::unique_ptr<BaseExpr>> m_vars;
};

class WriteStmt : public BaseStmt {
 public:
  explicit WriteStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

  WriteStmt() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "WriteStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level++);
    for (const auto &expr : m_exprs) {
      expr->print(output, level);
    }
  }

  void addExpr(std::unique_ptr<BaseExpr> expr) {
    m_exprs.push_back(std::move(expr));
  }

  const std::vector<std::unique_ptr<BaseExpr>> &getExprs() const {
    return m_exprs;
  }

 private:
  std::vector<std::unique_ptr<BaseExpr>> m_exprs;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_STATEMENTS_HPP
