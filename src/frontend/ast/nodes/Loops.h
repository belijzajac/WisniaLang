#ifndef AST_LOOPS_H
#define AST_LOOPS_H

// Wisnia
#include "Statements.h"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {
// An abstract definition for Loop node
class Loop : public Stmt {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level);
  }

  void addBody(std::unique_ptr<Stmt> body) {
    body_ = std::move(body);
  }

 public:
  std::unique_ptr<Stmt> body_;  // surrounded by "{" and "}"
};

// While loop statement node
class WhileLoop : public Loop {
 public:
  explicit WhileLoop(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  WhileLoop() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "WhileLoop";
  }

  void print(size_t level) const override {
    Loop::print(level); level++;
    cond_->print(level);
    body_->print(level);
  }

  void addCond(std::unique_ptr<Expr> expr) {
    cond_ = std::move(expr);
  }

 public:
  std::unique_ptr<Expr> cond_;
};

// For loop statement node
class ForLoop : public Loop {
 public:
  explicit ForLoop(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ForLoop() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForLoop";
  }

  void print(size_t level) const override {
    Loop::print(level); level++;
    init_->print(level);
    cond_->print(level);
    incdec_->print(level);
    body_->print(level);
  }

  void addInit(std::unique_ptr<Stmt> expr) {
    init_ = std::move(expr);
  }

  void addCond(std::unique_ptr<Expr> expr) {
    cond_ = std::move(expr);
  }

  void addIncDec(std::unique_ptr<Stmt> stmt) {
    incdec_ = std::move(stmt);
  }

 public:
  std::unique_ptr<Stmt> init_;
  std::unique_ptr<Expr> cond_;
  std::unique_ptr<Stmt> incdec_;
};

// ForEach loop statement node
class ForEachLoop : public Loop {
 public:
  explicit ForEachLoop(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  ForEachLoop() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForEachLoop";
  }

  void print(size_t level) const override {
    Loop::print(level); level++;
    elem_->print(level);
    iterElem_->print(level);
    body_->print(level);
  }

  void addElem(std::unique_ptr<Expr> expr) {
    elem_ = std::move(expr);
  }

  void addIterElem(std::unique_ptr<Expr> expr) {
    iterElem_ = std::move(expr);
  }

 public:
  std::unique_ptr<Expr> elem_;      // element
  std::unique_ptr<Expr> iterElem_;  // iterable element
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_LOOPS_H
