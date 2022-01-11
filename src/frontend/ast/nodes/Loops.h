#ifndef AST_LOOPS_H
#define AST_LOOPS_H

// Wisnia
#include "Root.h"
#include "Statements.h"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class BaseLoop : public BaseStmt {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    BaseStmt::print(level);
  }

  void addBody(std::unique_ptr<BaseStmt> body) {
    m_body = std::move(body);
  }

 public:
  std::unique_ptr<BaseStmt> m_body; // surrounded by "{" and "}"
};

class WhileLoop : public BaseLoop {
 public:
  explicit WhileLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "WhileLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_cond->print(level);
    m_body->print(level);
  }

  void addCond(std::unique_ptr<BaseExpr> expr) {
    m_cond = std::move(expr);
  }

 public:
  std::unique_ptr<BaseExpr> m_cond;
};

class ForLoop : public BaseLoop {
 public:
  explicit ForLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_initialization->print(level);
    m_condition->print(level);
    m_increment->print(level);
    m_body->print(level);
  }

  void addInit(std::unique_ptr<BaseStmt> expr) {
    m_initialization = std::move(expr);
  }

  void addCond(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  void addInc(std::unique_ptr<BaseStmt> stmt) {
    m_increment = std::move(stmt);
  }

 public:
  std::unique_ptr<BaseStmt> m_initialization;
  std::unique_ptr<BaseExpr> m_condition;
  std::unique_ptr<BaseStmt> m_increment;
};

class ForEachLoop : public BaseLoop {
 public:
  explicit ForEachLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForEachLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_element->print(level);
    m_collection->print(level);
    m_body->print(level);
  }

  void addElement(std::unique_ptr<BaseExpr> expr) {
    m_element = std::move(expr);
  }

  void addCollection(std::unique_ptr<BaseExpr> expr) {
    m_collection = std::move(expr);
  }

 public:
  std::unique_ptr<BaseExpr> m_element;
  std::unique_ptr<BaseExpr> m_collection;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_LOOPS_H
