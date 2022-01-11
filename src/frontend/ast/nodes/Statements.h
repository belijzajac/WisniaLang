#ifndef AST_STATEMENTS_H
#define AST_STATEMENTS_H

#include <string>
// Wisnia
#include "Root.h"
#include "Types.h"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class BaseStmt : public Root {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level);
  }
};

class StmtBlock : public BaseStmt {
 public:
  explicit StmtBlock(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  StmtBlock() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "StmtBlock";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    for (const auto &stmt : m_statements)
      stmt->print(level);
  }

  void addStmt(std::unique_ptr<BaseStmt> stmt) {
    m_statements.push_back(std::move(stmt));
  }

 public:
  std::vector<std::unique_ptr<BaseStmt>> m_statements;
};

class ReturnStmt : public BaseStmt {
 public:
  explicit ReturnStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  ReturnStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ReturnStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    m_returnValue->print(level);
  }

  void addReturnValue(std::unique_ptr<BaseExpr> returnVal) {
    m_returnValue = std::move(returnVal);
  }

 public:
  std::unique_ptr<BaseExpr> m_returnValue;
};

class BreakStmt : public BaseStmt {
 public:
  explicit BreakStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "BreakStmt";
  }
};

class ContinueStmt : public BaseStmt {
 public:
  explicit ContinueStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ContinueStmt";
  }
};

class VarDeclStmt : public BaseStmt {
 public:
  explicit VarDeclStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  VarDeclStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "VarDeclStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    m_var->print(level);
    if (m_value) m_value->print(level);
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

 public:
  std::unique_ptr<BaseExpr> m_var;   // name + type
  std::unique_ptr<BaseExpr> m_value; // variable's value
};

class VarAssignStmt : public BaseStmt {
 public:
  explicit VarAssignStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  VarAssignStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "VarAssignStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    m_var->print(level);
    m_val->print(level);
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

 public:
  std::unique_ptr<BaseExpr> m_var; // name + type
  std::unique_ptr<BaseExpr> m_val; // variable's value
};

class ExprStmt : public BaseStmt {
 public:
  explicit ExprStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  ExprStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ExprStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    m_expr->print(level);
  }

  void addExpr(std::unique_ptr<BaseExpr> expr) {
    m_expr = std::move(expr);
  }

 public:
  std::unique_ptr<BaseExpr> m_expr;
};

class ReadStmt : public BaseStmt {
 public:
  explicit ReadStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  ReadStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ReadStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    for (const auto &var : m_vars)
      var->print(level);
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_vars.push_back(std::move(var));
  }

 public:
  std::vector<std::unique_ptr<BaseExpr>> m_vars;
};

class WriteStmt : public BaseStmt {
 public:
  explicit WriteStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
  WriteStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "WriteStmt";
  }

  void print(size_t level) const override {
    BaseStmt::print(level++);
    for (const auto &expr : m_exprs)
      expr->print(level);
  }

  void addExpr(std::unique_ptr<BaseExpr> expr) {
    m_exprs.push_back(std::move(expr));
  }

 public:
  std::vector<std::unique_ptr<BaseExpr>> m_exprs;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_STATEMENTS_H
