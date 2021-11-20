#ifndef AST_CONDITIONALS_H
#define AST_CONDITIONALS_H

// Wisnia
#include "Statements.h"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

// An abstract definition for if statement node
class Cond : public Stmt {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Stmt::print(level);
  }

  void addBody(std::unique_ptr<Stmt> body) {
    body_ = std::move(body);
  }

 public:
  std::unique_ptr<Stmt> body_;  // surrounded by "{" and "}"
};

// If statement node
class IfStmt : public Cond {
 public:
  explicit IfStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  IfStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "IfStmt";
  }

  void print(size_t level) const override {
    Cond::print(level); level++;
    cond_->print(level);
    body_->print(level);
    level--; // reset for else statements
    for (const auto &elseBl : elseBlcks_)
      elseBl->print(level);
  }

  void addCond(std::unique_ptr<Expr> expr) {
    cond_ = std::move(expr);
  }

  void addElseBlocks(std::vector<std::unique_ptr<Cond>> expr) {
    elseBlcks_ = std::move(expr);
  }

 public:
  std::unique_ptr<Expr> cond_;                    // if condition
  std::vector<std::unique_ptr<Cond>> elseBlcks_;  // else blocks
};

// Else statement node
class ElseStmt : public Cond {
 public:
  explicit ElseStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ElseStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseStmt";
  }

  void print(size_t level) const override {
    Cond::print(level); level++;
    body_->print(level);
  }
};

// Else If statement node
class ElseIfStmt : public Cond {
 public:
  explicit ElseIfStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ElseIfStmt() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseIfStmt";
  }

  void print(size_t level) const override {
    Cond::print(level); level++;
    cond_->print(level);
    body_->print(level);
  }

  void addCond(std::unique_ptr<Expr> expr) {
    cond_ = std::move(expr);
  }

 public:
  std::unique_ptr<Expr> cond_;  // else if condition
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_CONDITIONALS_H
