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
class BaseIf : public Stmt {
 protected:
  std::unique_ptr<Stmt> body_;  // surrounded by "{" and "}"
 public:
  void print(size_t level) const override {
    Stmt::print(level);
    level++;
    body_->print(level);
  }

  // Mutators
  void addBody(std::unique_ptr<Stmt> body) { body_ = std::move(body); }
};

// If statement node
class IfStmt : public BaseIf {
  std::unique_ptr<Expr> cond_;                      // if condition
  std::vector<std::unique_ptr<BaseIf>> elseBlcks_;  // else blocks
 public:
  explicit IfStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  IfStmt() = default;

  std::string kind() const override { return "IfStmt"; }

  void print(size_t level) const override {
    // Print header
    printf("%s%s\n", std::string(level * 2, ' ').c_str(), kind().c_str());
    level++;

    // Print the if insides
    cond_->print(level);
    body_->print(level);

    for (const auto &elseBl : elseBlcks_)
      elseBl->print(level);
  }

  // Mutators
  void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
  void addElseBlocks(std::vector<std::unique_ptr<BaseIf>> expr) { elseBlcks_ = std::move(expr); }
};

// Else statement node
class ElseStmt : public BaseIf {
 public:
  explicit ElseStmt(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ElseStmt() = default;

  std::string kind() const override { return "ElseStmt"; }

  void print(size_t level) const override { BaseIf::print(level); }
};

// Else If statement node
class ElseIfStmt : public BaseIf {
  std::unique_ptr<Expr> cond_;  // else if condition
 public:
  explicit ElseIfStmt(const std::shared_ptr<Basic::Token> &tok) {
    token_ = tok;
  }
  ElseIfStmt() = default;

  std::string kind() const override { return "ElseIfStmt"; }

  void print(size_t level) const override {
    // Print header
    printf("%s%s\n", std::string(level * 2, ' ').c_str(), kind().c_str());
    level++;

    // Print the if insides
    cond_->print(level);
    body_->print(level);
  }

  // Mutators
  void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_CONDITIONALS_H
