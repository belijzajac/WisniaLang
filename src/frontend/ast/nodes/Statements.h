#ifndef AST_STATEMENTS_H
#define AST_STATEMENTS_H

#include <string>
// Wisnia
#include "Root.h"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {
// An abstract definition for Stmt (statement) node
class Stmt : public Root {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level);
  }
};

// Statement block node
class StmtBlock : public Stmt {
 public:
  explicit StmtBlock(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  StmtBlock() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "StmtBlock";
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    for (const auto &stmt : stmts_)
      stmt->print(level);
  }

  void addStmt(std::unique_ptr<Stmt> stmt) {
    stmts_.push_back(std::move(stmt));
  }

 public:
  std::vector<std::unique_ptr<Stmt>> stmts_;
};

// Return statement node
class ReturnStmt : public Stmt {
 public:
  explicit ReturnStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ReturnStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ReturnStmt";
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    returnValue_->print(level);
  }

  void addReturnValue(std::unique_ptr<Expr> returnVal) {
    returnValue_ = std::move(returnVal);
  }

 public:
  std::unique_ptr<Expr> returnValue_;
};

// Loop break statement node
class BreakStmt : public Stmt {
 public:
  explicit BreakStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  BreakStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "BreakStmt";
  }
};

// Variable declaration statement node
class VarDeclStmt : public Stmt {
 public:
  explicit VarDeclStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  VarDeclStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "VarDeclStmt" << " (" << name_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    if (type_) type_->print(level);
    if (value_) value_->print(level);
  }

  void addType(std::unique_ptr<Type> varType) {
    type_ = std::move(varType);
  }

  void addName(std::shared_ptr<Basic::Token> varName) {
    name_ = varName;
  }

  void addValue(std::unique_ptr<Expr> varValue) {
    value_ = std::move(varValue);
  }

 public:
  std::unique_ptr<Type> type_;          // variable type
  std::shared_ptr<Basic::Token> name_;  // variable name
  std::unique_ptr<Expr> value_;         // variable value
};

// Variable assignment statement node
class VarAssignStmt : public Stmt {
 public:
  explicit VarAssignStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  VarAssignStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "VarAssignStmt" << " (" << name_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    value_->print(level);
  }

  void addName(std::shared_ptr<Basic::Token> varName) {
    name_ = varName;
  }

  void addValue(std::unique_ptr<Expr> varValue) {
    value_ = std::move(varValue);
  }

 public:
  std::shared_ptr<Basic::Token> name_;  // variable name
  std::unique_ptr<Expr> value_;         // variable value
};

// Expression statement node
class ExprStmt : public Stmt {
 public:
  explicit ExprStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ExprStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ExprStmt";
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    expr_->print(level);
  }

  void addExpr(std::unique_ptr<Expr> expr) {
    expr_ = std::move(expr);
  }

 public:
  std::unique_ptr<Expr> expr_;
};

// Read IO statement node
class ReadStmt : public Stmt {
 public:
  explicit ReadStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ReadStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ReadStmt";
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    for (const auto &var : vars_)
      var->print(level);
  }

  void addVar(std::unique_ptr<Expr> var) {
    vars_.push_back(std::move(var));
  }

 public:
  std::vector<std::unique_ptr<Expr>> vars_;
};

// Write IO statement node
class WriteStmt : public Stmt {
 public:
  explicit WriteStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  WriteStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "WriteStmt";
  }

  void print(size_t level) const override {
    Stmt::print(level); level++;
    for (const auto &expr : exprs_)
      expr->print(level);
  }

  void addExpr(std::unique_ptr<Expr> expr) {
    exprs_.push_back(std::move(expr));
  }

 public:
  std::vector<std::unique_ptr<Expr>> exprs_;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_STATEMENTS_H
