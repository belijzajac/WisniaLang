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
  void print(size_t level) const override { Root::print(level); }
};

// Statement block node
class StmtBlock : public Stmt {
  std::vector<std::unique_ptr<Stmt>> stmts_;
 public:
  explicit StmtBlock(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  StmtBlock() = default;

  void addStmt(std::unique_ptr<Stmt> stmt) {
    stmts_.push_back(std::move(stmt));
  }

  std::string kind() const override { return "StmtBlock"; }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;

    for (const auto &stmt : stmts_)
      stmt->print(level);
  }
};

// Return statement node
class ReturnStmt : public Stmt {
  std::unique_ptr<Expr> returnValue_;
 public:
  explicit ReturnStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ReturnStmt() = default;

  void addReturnValue(std::unique_ptr<Expr> returnVal) {
    returnValue_ = std::move(returnVal);
  }

  std::string kind() const override { return "ReturnStmt"; }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;

    if (returnValue_)
      returnValue_->print(level);
  }
};

// Loop break statement node
class LoopBrkStmt : public Stmt {
 public:
  explicit LoopBrkStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  LoopBrkStmt() = default;

  std::string kind() const override { return "LoopBrkStmt"; }
};

// Variable declaration statement node
class VarDeclStmt : public Stmt {
  std::unique_ptr<Type> type_;          // variable type
  std::shared_ptr<Basic::Token> name_;  // variable name
  std::unique_ptr<Expr> value_;         // variable value
 public:
  explicit VarDeclStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  VarDeclStmt() = default;

  std::string kind() const override {
    std::stringstream ss;
    ss << "VarDeclStmt" << " (" << name_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;

    if (type_) type_->print(level);
    if (value_) value_->print(level);
  }

  // Mutators
  void addType(std::unique_ptr<Type> varType) { type_ = std::move(varType); }
  void addName(std::shared_ptr<Basic::Token> varName) { name_ = varName; }
  void addValue(std::unique_ptr<Expr> varValue) { value_ = std::move(varValue); }
};

// Variable assignment statement node
class VarAssignStmt : public Stmt {
  std::shared_ptr<Basic::Token> name_;  // variable name
  std::unique_ptr<Expr> value_;         // variable value
 public:
  explicit VarAssignStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  VarAssignStmt() = default;

  std::string kind() const override {
    std::stringstream ss;
    ss << "VarAssignStmt" << " (" << name_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;
    value_->print(level);
  }

  // Mutators
  void addName(std::shared_ptr<Basic::Token> varName) { name_ = varName; }
  void addValue(std::unique_ptr<Expr> varValue) {
    value_ = std::move(varValue);
  }
};

// Expression statement node
class ExprStmt : public Stmt {
  std::unique_ptr<Expr> expr_;
 public:
  explicit ExprStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ExprStmt() = default;

  std::string kind() const override { return "ExprStmt"; }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;
    expr_->print(level);
  }

  // Mutators
  void addExpr(std::unique_ptr<Expr> expr) { expr_ = std::move(expr); }
};

// Read IO statement node
class readIOStmt : public Stmt {
  std::vector<std::unique_ptr<Var>> vars_;
 public:
  explicit readIOStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  readIOStmt() = default;

  std::string kind() const override { return "readIOStmt"; }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;

    for (const auto &var : vars_)
      var->print(level);
  }

  // Mutators
  void addVar(std::unique_ptr<Var> var) { vars_.push_back(std::move(var)); }
};

// Write IO statement node
class writeIOStmt : public Stmt {
  std::vector<std::unique_ptr<Expr>> exprs_;
 public:
  explicit writeIOStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  writeIOStmt() = default;

  std::string kind() const override { return "writeIOStmt"; }

  void print(size_t level) const override {
    Stmt::print(level);
    level++;

    for (const auto &expr : exprs_)
      expr->print(level);
  }

  // Mutators
  void addExpr(std::unique_ptr<Expr> expr) {
    exprs_.push_back(std::move(expr));
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_STATEMENTS_H
